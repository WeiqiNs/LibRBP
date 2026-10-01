#include <algorithm>
#include <memory>
#include "relic.hpp"

namespace rbp{
    namespace{
        template <Side S>
        struct Ops;

        template <>
        struct Ops<Side::g1>{
            using Element = detail::G1Element;
            using Array = g1_t;
            static constexpr std::uint8_t domain_byte = 1;
            static constexpr std::string_view name = "G1";

            static void identity(Element* p){ g1_set_infty(p); }
            static void generator(Element* p){ g1_get_gen(p); }
            static void random(Element* p){ g1_rand(p); }
            static void map(Element* p, const Bytes& input){ g1_map(p, input.data(), input.size()); }
            static void mul_generator(Element* p, const bn_st* k){ g1_mul_gen(p, k); }
            static void add(Element* r, const Element* p, const Element* q){ g1_add(r, p, q); }
            static void neg(Element* r, const Element* p){ g1_neg(r, p); }
            static void mul(Element* r, const Element* p, const bn_st* k){ g1_mul(r, p, k); }
            static void copy(Element* r, const Element* p){ g1_copy(r, p); }
            static void mul_sim_lot(Element* r, const Array* p, const bn_t* k, const std::size_t n){
                g1_mul_sim_lot(r, p, k, n);
            }
            static bool equal(const Element* p, const Element* q){ return g1_cmp(p, q) == RLC_EQ; }
            static bool is_identity(const Element* p){ return g1_is_infty(p); }
            static bool is_valid(const Element* p){ return g1_is_valid(p); }
            static std::size_t size_bin(const Element* p, const bool pack){ return g1_size_bin(p, pack); }
            static void write_bin(Bytes& out, const Element* p, const bool pack){
                g1_write_bin(out.data(), out.size(), p, pack);
            }
            static void read_bin(Element* p, const ByteView in){ g1_read_bin(p, in.data(), in.size()); }
        };

        template <>
        struct Ops<Side::g2>{
            using Element = detail::G2Element;
            using Array = g2_t;
            static constexpr std::uint8_t domain_byte = 2;
            static constexpr std::string_view name = "G2";

            static void identity(Element* p){ g2_set_infty(p); }
            static void generator(Element* p){ g2_get_gen(p); }
            static void random(Element* p){ g2_rand(p); }
            static void map(Element* p, const Bytes& input){ g2_map(p, input.data(), input.size()); }
            static void mul_generator(Element* p, const bn_st* k){ g2_mul_gen(p, k); }
            static void add(Element* r, const Element* p, const Element* q){ g2_add(r, p, q); }
            static void neg(Element* r, const Element* p){ g2_neg(r, p); }
            static void mul(Element* r, const Element* p, const bn_st* k){ g2_mul(r, p, k); }
            static void copy(Element* r, const Element* p){ g2_copy(r, p); }
            static void mul_sim_lot(Element* r, const Array* p, const bn_t* k, const std::size_t n){
                g2_mul_sim_lot(r, p, k, n);
            }
            static bool equal(const Element* p, const Element* q){ return g2_cmp(p, q) == RLC_EQ; }
            static bool is_identity(const Element* p){ return g2_is_infty(p); }
            static bool is_valid(const Element* p){ return g2_is_valid(p); }
            static std::size_t size_bin(const Element* p, const bool pack){ return g2_size_bin(p, pack); }
            static void write_bin(Bytes& out, const Element* p, const bool pack){
                g2_write_bin(out.data(), out.size(), p, pack);
            }
            static void read_bin(Element* p, const ByteView in){ g2_read_bin(p, in.data(), in.size()); }
        };

        Bytes domain_separated(const std::uint8_t side, const std::string_view domain, const ByteView message){
            Bytes input;
            for (int shift = 56; shift >= 0; shift -= 8)
                input.push_back(static_cast<std::uint8_t>(domain.size() >> shift));
            input.insert(input.end(), domain.begin(), domain.end());
            input.push_back(side);
            input.insert(input.end(), message.begin(), message.end());
            return input;
        }
    }

    template <class C, Side S>
    Point<C, S>::Point(){
        detail::Runtime<C>::require();
        Ops<S>::identity(raw(*this));
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::generator(){
        Point r;
        Ops<S>::generator(raw(r));
        return r;
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::random(){
        Point r;
        Ops<S>::random(raw(r));
        return r;
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::hash(const std::string_view domain, const ByteView message){
        Point r;
        Ops<S>::map(raw(r), domain_separated(Ops<S>::domain_byte, domain, message));
        return r;
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::mul_generator(const Zp<C>& scalar){
        Point r;
        Ops<S>::mul_generator(raw(r), raw(scalar));
        return r;
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::from_bytes(const ByteView bytes){
        Point r;
        if (bytes.size() == 1 && bytes.front() == 0) return r;
        clear_relic_error();
        Ops<S>::read_bin(raw(r), bytes);
        if (relic_failed() || !Ops<S>::is_valid(raw(r))){
            throw DecodeError(std::string(Ops<S>::name) + " encoding is not a point of the prime-order subgroup");
        }
        return r;
    }

    template <class C, Side S>
    Bytes Point<C, S>::to_bytes(const Encoding encoding) const{
        if (is_identity()) return Bytes{0};
        const bool pack = encoding == Encoding::compressed;
        Bytes out(Ops<S>::size_bin(raw(*this), pack));
        Ops<S>::write_bin(out, raw(*this), pack);
        return out;
    }

    template <class C, Side S>
    bool Point<C, S>::is_identity() const{
        return Ops<S>::is_identity(raw(*this));
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::plus(const Point& q) const{
        Point r;
        Ops<S>::add(raw(r), raw(*this), raw(q));
        return r;
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::times(const Zp<C>& k) const{
        Point r;
        Ops<S>::mul(raw(r), raw(*this), raw(k));
        return r;
    }

    template <class C, Side S>
    Point<C, S> Point<C, S>::negated() const{
        Point r;
        Ops<S>::neg(raw(r), raw(*this));
        return r;
    }

    template <class C, Side S>
    bool Point<C, S>::equals(const Point& q) const{
        return Ops<S>::equal(raw(*this), raw(q));
    }

    template <class C, Side S>
    Point<C, S> msm(const std::vector<Point<C, S>>& points, const std::vector<Zp<C>>& scalars){
        if (points.size() != scalars.size()) throw ShapeError("msm needs one scalar per point");
        Point<C, S> total;
        for (std::size_t start = 0; start < points.size(); start += batch_size){
            const auto count = std::min(batch_size, points.size() - start);
            const auto bases = std::make_unique<typename Ops<S>::Array[]>(count);
            const auto exponents = std::make_unique<bn_t[]>(count);
            for (std::size_t i = 0; i < count; ++i){
                Ops<S>::copy(bases[i], raw(points[start + i]));
                bn_new(exponents[i]);
                bn_copy(exponents[i], raw(scalars[start + i]));
            }
            Point<C, S> part;
            Ops<S>::mul_sim_lot(raw(part), bases.get(), exponents.get(), count);
            total += part;
        }
        return total;
    }

    template class Point<detail::Tag, Side::g1>;
    template class Point<detail::Tag, Side::g2>;
    template G1<detail::Tag> msm(const std::vector<G1<detail::Tag>>&, const std::vector<Zp<detail::Tag>>&);
    template G2<detail::Tag> msm(const std::vector<G2<detail::Tag>>&, const std::vector<Zp<detail::Tag>>&);
}
