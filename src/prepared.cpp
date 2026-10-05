#include <algorithm>
#include <cstdint>
#include <iterator>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
#include "relic.hpp"

#ifndef RLC_GT_EMBED
#error "prepared pairing needs RELIC's RLC_GT_EMBED"
#endif

#if RLC_GT_EMBED == 12
#if PP_MAP != OATEP || EP_ADD == BASIC
#error "prepared pairing replays RELIC's projective optimal-ate Miller loop"
#endif

namespace rbp{
    using detail::raw;

    namespace{
        static_assert(std::is_same_v<dig_t, std::uint64_t>);

        constexpr std::size_t fp_words = sizeof(fp_t) / sizeof(dig_t);
        constexpr std::size_t fp2_words = 2 * fp_words;
        constexpr std::size_t line_words = 3 * fp2_words;

        struct MillerSchedule{
            std::vector<std::int8_t> digits;
            bool negative;
            bool frobenius_lines;

            [[nodiscard]] std::size_t line_count() const{
                const auto additions = digits.size() - static_cast<std::size_t>(std::ranges::count(digits, 0));
                return digits.size() + additions + (frobenius_lines ? 2 : 0);
            }
        };

        struct LineLayout{
            int one;
            int zero;

            void store(std::uint64_t* out, const fp12_t line) const{
                for (const auto* slot : {line[one][one], line[zero][zero], line[one][zero]}){
                    fp_copy(out, slot[0]);
                    fp_copy(out + fp_words, slot[1]);
                    out += fp2_words;
                }
            }

            void evaluate(fp12_t line, const std::uint64_t* coefficients, const ep_st* p) const{
                fp_copy(line[one][one][0], coefficients);
                fp_copy(line[one][one][1], coefficients + fp_words);
                fp_mul(line[zero][zero][0], coefficients + 2 * fp_words, p->y);
                fp_mul(line[zero][zero][1], coefficients + 3 * fp_words, p->y);
                fp_mul(line[one][zero][0], coefficients + 4 * fp_words, p->x);
                fp_mul(line[one][zero][1], coefficients + 5 * fp_words, p->x);
            }
        };

        struct LineWriter{
            const MillerSchedule& schedule;
            LineLayout layout;
            std::size_t stride;

            void record(const detail::G2Element* point, std::uint64_t* cursor) const{
                ep_t at_one, doubling_at_one;
                fp_set_dig(at_one->x, 1);
                fp_set_dig(at_one->y, 1);
                fp_set_dig(at_one->z, 1);
                at_one->coord = BASIC;
                fp_add(doubling_at_one->x, at_one->x, at_one->x);
                fp_add(doubling_at_one->x, doubling_at_one->x, at_one->x);
                fp_neg(doubling_at_one->y, at_one->y);
                fp_copy(doubling_at_one->z, at_one->z);
                doubling_at_one->coord = BASIC;

                ep2_t q, minus_q, t;
                ep2_norm(q, point);
                ep2_neg(minus_q, q);
                ep2_copy(t, q);
                fp12_t line;
                fp12_zero(line);
                const auto emit = [&]{
                    layout.store(cursor, line);
                    cursor += stride;
                };
                for (const auto digit : schedule.digits){
                    pp_dbl_k12(line, t, t, doubling_at_one);
                    emit();
                    if (digit > 0){
                        pp_add_k12(line, t, q, at_one);
                        emit();
                    }
                    if (digit < 0){
                        pp_add_k12(line, t, minus_q, at_one);
                        emit();
                    }
                }
                if (!schedule.frobenius_lines) return;
                if (schedule.negative) ep2_neg(t, t);
                ep2_t q1, q2;
                ep2_frb(q1, q, 1);
                ep2_frb(q2, q, 2);
                ep2_neg(q2, q2);
                pp_add_k12(line, t, q1, at_one);
                emit();
                pp_add_k12(line, t, q2, at_one);
                emit();
            }
        };

        struct ActivePoint{
            std::size_t index;
            ep_t p;
        };

        struct LineReader{
            LineLayout layout;
            std::span<const ActivePoint> active;

            void multiply(fp12_t r, const std::uint64_t* row) const{
                fp12_t line;
                fp12_zero(line);
                for (const auto& point : active){
                    layout.evaluate(line, row + point.index * line_words, point.p);
                    fp12_mul_dxs(r, r, line);
                }
            }
        };

        MillerSchedule miller_schedule(){
            bn_t a;
            bn_new(a);
            fp_prime_get_par(a);
            const bool bn_curve = ep_curve_is_pairf() == EP_BN;
            if (bn_curve){
                bn_mul_dig(a, a, 6);
                bn_add_dig(a, a, 2);
            }
            std::int8_t naf[RLC_FP_BITS + 1];
            std::size_t length = bn_bits(a) + 1;
            bn_rec_naf(naf, &length, a, 2);
            std::vector<std::int8_t> digits(
                std::make_reverse_iterator(naf + length - 1), std::make_reverse_iterator(naf));
            return {.digits = std::move(digits), .negative = bn_sign(a) == RLC_NEG, .frobenius_lines = bn_curve};
        }

        LineLayout line_layout(){
            if (ep2_curve_is_twist() == RLC_EP_MTYPE) return {.one = 0, .zero = 1};
            return {.one = 1, .zero = 0};
        }
    }

    template <class C>
    PreparedG2<C>::PreparedG2(std::vector<G2<C>> points) : points_(std::move(points)){
        detail::Runtime<C>::require();
        const auto schedule = miller_schedule();
        const LineWriter writer{.schedule = schedule, .layout = line_layout(), .stride = points_.size() * line_words};
        lines_.assign(schedule.line_count() * writer.stride, 0);
        for (std::size_t i = 0; i < points_.size(); ++i){
            if (!points_[i].is_identity()) writer.record(raw(points_[i]), lines_.data() + i * line_words);
        }
    }

    template <class C>
    Gt<C> pair(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs){
        const auto& points = detail::Raw::points(qs);
        if (ps.size() != points.size()) throw ShapeError("multi-pairing needs one G2 point per G1 point");
        std::vector<ActivePoint> active;
        for (std::size_t i = 0; i < ps.size(); ++i){
            if (ps[i].is_identity() || points[i].is_identity()) continue;
            ep_norm(active.emplace_back(ActivePoint{.index = i, .p = {}}).p, raw(ps[i]));
        }
        Gt<C> result;
        if (active.empty()) return result;

        const auto schedule = miller_schedule();
        const LineReader reader{.layout = line_layout(), .active = active};
        const auto stride = points.size() * line_words;
        const auto* row = detail::Raw::lines(qs).data();
        const auto next_row = [&]{ return std::exchange(row, row + stride); };
        const auto r = raw(result);
        for (std::size_t step = 0; step < schedule.digits.size(); ++step){
            if (step > 0) fp12_sqr(r, r);
            reader.multiply(r, next_row());
            if (schedule.digits[step] != 0) reader.multiply(r, next_row());
        }
        if (schedule.negative) fp12_inv_cyc(r, r);
        if (schedule.frobenius_lines){
            reader.multiply(r, next_row());
            reader.multiply(r, next_row());
        }
        pp_exp_k12(r, r);
        return result;
    }

    template class PreparedG2<detail::Tag>;
    template Gt<detail::Tag> pair(const std::vector<G1<detail::Tag>>&, const PreparedG2<detail::Tag>&);
}

#else

namespace rbp{
    template <class C>
    PreparedG2<C>::PreparedG2(std::vector<G2<C>> points) : points_(std::move(points)){
        detail::Runtime<C>::require();
    }

    template <class C>
    Gt<C> pair(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs){
        return pair(ps, detail::Raw::points(qs));
    }

    template class PreparedG2<detail::Tag>;
    template Gt<detail::Tag> pair(const std::vector<G1<detail::Tag>>&, const PreparedG2<detail::Tag>&);
}

#endif
