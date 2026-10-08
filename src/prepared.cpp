#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include "relic.hpp"

#ifndef RLC_GT_EMBED
#error "prepared pairing needs RELIC's RLC_GT_EMBED"
#endif

namespace rbp{
    using detail::raw;

    namespace{
        void require_one_g2_per_g1(const std::size_t g1_count, const std::size_t g2_count){
            if (g1_count != g2_count) throw ShapeError("a pairing product needs one G2 point per G1 point");
        }
    }

    template <class C>
    void PairingProduct<C>::add(const G1<C>& p, const G2<C>& q){
        ps_.push_back(p);
        qs_.push_back(q);
    }

    template <class C>
    void PairingProduct<C>::add(const std::vector<G1<C>>& ps, const std::vector<G2<C>>& qs){
        require_one_g2_per_g1(ps.size(), qs.size());
        ps_.insert(ps_.end(), ps.begin(), ps.end());
        qs_.insert(qs_.end(), qs.begin(), qs.end());
    }

    template <class C>
    void PairingProduct<C>::add(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs){
        require_one_g2_per_g1(ps.size(), detail::Raw::points(qs).size());
        prepared_.push_back({.ps = ps, .qs = &qs});
    }
}

#if RLC_GT_EMBED == 12
#if PP_MAP != OATEP || EP_ADD == BASIC
#error "prepared pairing replays RELIC's projective optimal-ate Miller loop"
#endif

namespace rbp{
    namespace{
        static_assert(std::is_same_v<dig_t, std::uint64_t>);

        constexpr std::size_t fp_words = sizeof(fp_t) / sizeof(dig_t);
        constexpr std::size_t fp2_words = 2 * fp_words;
        constexpr std::size_t line_words = 3 * fp2_words;

        enum class LineKind{ doubling, addition, subtraction, frobenius, frobenius_square };

        struct MillerSchedule{
            std::vector<std::int8_t> digits;
            bool negative;
            bool frobenius_lines;

            template <class Visitor>
            void walk(Visitor& visitor) const{
                for (const auto digit : digits){
                    visitor.square();
                    visitor.line(LineKind::doubling);
                    if (digit > 0) visitor.line(LineKind::addition);
                    if (digit < 0) visitor.line(LineKind::subtraction);
                }
                if (negative) visitor.conjugate();
                if (!frobenius_lines) return;
                visitor.line(LineKind::frobenius);
                visitor.line(LineKind::frobenius_square);
            }

            [[nodiscard]] std::size_t line_count() const{
                struct Counter{
                    std::size_t lines = 0;

                    void square(){}
                    void conjugate(){}
                    void line(LineKind){ ++lines; }
                } counter;
                walk(counter);
                return counter.lines;
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

        struct LineStepper{
            ep_t p;
            ep_t doubling_p;
            ep2_t q;
            ep2_t minus_q;
            ep2_t t;

            void line(fp12_t out, const LineKind kind){
                ep2_t frobenius_q;
                switch (kind){
                    case LineKind::doubling:
                        pp_dbl_k12(out, t, t, doubling_p);
                        return;
                    case LineKind::addition:
                        pp_add_k12(out, t, q, p);
                        return;
                    case LineKind::subtraction:
                        pp_add_k12(out, t, minus_q, p);
                        return;
                    case LineKind::frobenius:
                        ep2_frb(frobenius_q, q, 1);
                        pp_add_k12(out, t, frobenius_q, p);
                        return;
                    case LineKind::frobenius_square:
                        ep2_frb(frobenius_q, q, 2);
                        ep2_neg(frobenius_q, frobenius_q);
                        pp_add_k12(out, t, frobenius_q, p);
                        return;
                }
            }

            void conjugate(){ ep2_neg(t, t); }
        };

        LineStepper line_stepper(const ep_st* p, const detail::G2Element* q){
            LineStepper stepper;
            ep_copy(stepper.p, p);
            ep_copy(stepper.doubling_p, p);
            fp_add(stepper.doubling_p->x, p->x, p->x);
            fp_add(stepper.doubling_p->x, stepper.doubling_p->x, p->x);
            fp_neg(stepper.doubling_p->y, p->y);
            ep2_norm(stepper.q, q);
            ep2_neg(stepper.minus_q, stepper.q);
            ep2_copy(stepper.t, stepper.q);
            return stepper;
        }

        struct LineWriter{
            LineLayout layout;
            LineStepper stepper;
            std::uint64_t* cursor;
            std::size_t stride;
            fp12_t buffer;

            void square(){}
            void conjugate(){ stepper.conjugate(); }

            void line(const LineKind kind){
                stepper.line(buffer, kind);
                layout.store(cursor, buffer);
                cursor += stride;
            }
        };

        struct ActivePoint{
            std::size_t index;
            ep_t p;
        };

        struct LineSource{
            std::vector<ActivePoint> active;
            const std::uint64_t* row;
            std::size_t stride;
        };

        struct ProductLoop{
            detail::GtElement* r;
            LineLayout layout;
            std::vector<LineSource> prepared;
            std::vector<LineStepper> variable;
            fp12_t buffer;

            void square(){ fp12_sqr(r, r); }

            void conjugate(){
                fp12_inv_cyc(r, r);
                for (auto& stepper : variable) stepper.conjugate();
            }

            void line(const LineKind kind){
                for (auto& source : prepared){
                    for (const auto& point : source.active){
                        layout.evaluate(buffer, source.row + point.index * line_words, point.p);
                        fp12_mul_dxs(r, r, buffer);
                    }
                    source.row += source.stride;
                }
                for (auto& stepper : variable){
                    stepper.line(buffer, kind);
                    fp12_mul_dxs(r, r, buffer);
                }
            }
        };

        template <class C>
        LineSource line_source(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs){
            const auto& points = detail::Raw::points(qs);
            LineSource source{.active = {}, .row = detail::Raw::lines(qs).data(), .stride = points.size() * line_words};
            for (std::size_t i = 0; i < ps.size(); ++i){
                if (ps[i].is_identity() || points[i].is_identity()) continue;
                ep_norm(source.active.emplace_back(ActivePoint{.index = i, .p = {}}).p, raw(ps[i]));
            }
            return source;
        }

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
        const auto stride = points_.size() * line_words;
        lines_.assign(schedule.line_count() * stride, 0);
        const auto layout = line_layout();
        ep_t at_one;
        fp_set_dig(at_one->x, 1);
        fp_set_dig(at_one->y, 1);
        fp_set_dig(at_one->z, 1);
        at_one->coord = BASIC;
        for (std::size_t i = 0; i < points_.size(); ++i){
            if (points_[i].is_identity()) continue;
            LineWriter writer{
                .layout = layout, .stepper = line_stepper(at_one, raw(points_[i])),
                .cursor = lines_.data() + i * line_words, .stride = stride, .buffer = {}
            };
            schedule.walk(writer);
        }
    }

    template <class C>
    Gt<C> PairingProduct<C>::evaluate() const{
        Gt<C> result;
        ProductLoop loop{.r = raw(result), .layout = line_layout(), .prepared = {}, .variable = {}, .buffer = {}};
        for (std::size_t i = 0; i < ps_.size(); ++i){
            if (ps_[i].is_identity() || qs_[i].is_identity()) continue;
            ep_t p;
            ep_norm(p, raw(ps_[i]));
            loop.variable.push_back(line_stepper(p, raw(qs_[i])));
        }
        for (const auto& term : prepared_){
            auto source = line_source(term.ps, *term.qs);
            if (!source.active.empty()) loop.prepared.push_back(std::move(source));
        }
        if (loop.prepared.empty() && loop.variable.empty()) return result;

        miller_schedule().walk(loop);
        pp_exp_k12(loop.r, loop.r);
        return result;
    }
}

#else

namespace rbp{
    template <class C>
    PreparedG2<C>::PreparedG2(std::vector<G2<C>> points) : points_(std::move(points)){
        detail::Runtime<C>::require();
    }

    template <class C>
    Gt<C> PairingProduct<C>::evaluate() const{
        auto ps = ps_;
        auto qs = qs_;
        for (const auto& term : prepared_){
            const auto& points = detail::Raw::points(*term.qs);
            ps.insert(ps.end(), term.ps.begin(), term.ps.end());
            qs.insert(qs.end(), points.begin(), points.end());
        }
        Gt<C> total;
        for (std::size_t start = 0; start < ps.size(); start += detail::batch_size){
            const auto count = std::min(detail::batch_size, ps.size() - start);
            const auto lefts = std::make_unique<g1_t[]>(count);
            const auto rights = std::make_unique<g2_t[]>(count);
            for (std::size_t i = 0; i < count; ++i){
                g1_copy(lefts[i], raw(ps[start + i]));
                g2_copy(rights[i], raw(qs[start + i]));
            }
            Gt<C> part;
            pc_map_sim(raw(part), lefts.get(), rights.get(), count);
            total *= part;
        }
        return total;
    }
}

#endif

namespace rbp{
    template class PreparedG2<detail::Tag>;
    template class PairingProduct<detail::Tag>;
}
