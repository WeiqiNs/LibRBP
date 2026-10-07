#include <cstring>
#include "relic.hpp"

namespace rbp{
    using detail::raw;
    using detail::clear_relic_error;
    using detail::relic_failed;

    template <class C>
    Gt<C>::Gt(){
        detail::Runtime<C>::require();
        gt_set_unity(raw(*this));
    }

    template <class C>
    Gt<C> Gt<C>::generator(){
        Gt r;
        gt_get_gen(raw(r));
        return r;
    }

    template <class C>
    Gt<C> Gt<C>::random(){
        Gt r;
        gt_rand(raw(r));
        return r;
    }

    template <class C>
    Gt<C> Gt<C>::from_bytes(const ByteView bytes){
        Gt r;
        const std::size_t encoded_size = gt_size_bin(raw(r), 0);
        if (bytes.size() != encoded_size) throw DecodeError("Gt encoding has the wrong length");
        clear_relic_error();
        gt_read_bin(raw(r), bytes.data(), bytes.size());
        if (relic_failed() || !(gt_is_unity(raw(r)) || gt_is_valid(raw(r)))){
            throw DecodeError("Gt encoding is not an element of the order-r subgroup");
        }
        return r;
    }

    template <class C>
    Bytes Gt<C>::to_bytes() const{
        Bytes out(gt_size_bin(raw(*this), 0));
        gt_write_bin(out.data(), out.size(), raw(*this), 0);
        return out;
    }

    template <class C>
    bool Gt<C>::is_one() const{
        return gt_is_unity(raw(*this));
    }

    template <class C>
    std::uint64_t Gt<C>::fingerprint() const{
        fp_t first;
        std::memcpy(first, raw(*this), sizeof(fp_t));
        fp_norm(first, first);
        return first[0];
    }

    template <class C>
    Gt<C> Gt<C>::inverse() const{
        Gt r;
        gt_inv(raw(r), raw(*this));
        return r;
    }

    template <class C>
    Gt<C> Gt<C>::pow(const Zp<C>& exponent) const{
        Gt r;
        gt_exp(raw(r), raw(*this), raw(exponent));
        return r;
    }

    template <class C>
    Gt<C> Gt<C>::times(const Gt& y) const{
        Gt r;
        gt_mul(raw(r), raw(*this), raw(y));
        return r;
    }

    template <class C>
    bool Gt<C>::equals(const Gt& y) const{
        return gt_cmp(raw(*this), raw(y)) == RLC_EQ;
    }

    template class Gt<detail::Tag>;
}
