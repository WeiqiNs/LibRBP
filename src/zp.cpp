#include "relic.hpp"

namespace rbp{
    namespace{
        template <class C>
        const bn_st* order(){ return detail::Runtime<C>::require().order(); }
    }

    template <class C>
    Zp<C>::Zp(){
        detail::Runtime<C>::require();
        bn_new(raw(*this));
    }

    template <class C>
    Zp<C> Zp<C>::from_unsigned(const std::uint64_t value){
        Zp r;
        bn_set_dig(raw(r), value);
        bn_mod(raw(r), raw(r), order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::from_signed(const std::int64_t value){
        const auto magnitude = static_cast<std::uint64_t>(value);
        return value < 0 ? -from_unsigned(0 - magnitude) : from_unsigned(magnitude);
    }

    template <class C>
    Zp<C> Zp<C>::random(){
        Zp r;
        bn_rand_mod(raw(r), order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::hash(const std::string_view domain, const ByteView message){
        const Bytes tag(domain.begin(), domain.end());
        Bytes wide(byte_size() + 16);
        md_xmd(wide.data(), wide.size(), message.data(), message.size(), tag.data(), tag.size());
        Zp r;
        bn_read_bin(raw(r), wide.data(), wide.size());
        bn_mod(raw(r), raw(r), order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::from_bytes(const ByteView bytes){
        if (bytes.size() != byte_size()) throw DecodeError("Zp encoding must be exactly byte_size() bytes");
        Zp r;
        bn_read_bin(raw(r), bytes.data(), bytes.size());
        if (bn_cmp(raw(r), order<C>()) != RLC_LT) throw DecodeError("Zp encoding is not below the group order");
        return r;
    }

    template <class C>
    std::size_t Zp<C>::byte_size(){
        return detail::Runtime<C>::require().order_bytes();
    }

    template <class C>
    Bytes Zp<C>::to_bytes() const{
        Bytes out(byte_size());
        bn_write_bin(out.data(), out.size(), raw(*this));
        return out;
    }

    template <class C>
    std::string Zp<C>::to_string() const{
        std::string out(bn_size_str(raw(*this), 10), '\0');
        bn_write_str(out.data(), out.size(), raw(*this), 10);
        out.pop_back();
        return out;
    }

    template <class C>
    bool Zp<C>::is_zero() const{
        return bn_is_zero(raw(*this));
    }

    template <class C>
    Zp<C> Zp<C>::inverse() const{
        if (is_zero()) throw NotInvertible("zero has no inverse in Zp");
        Zp r;
        bn_mod_inv(raw(r), raw(*this), order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::pow(const std::uint64_t exponent) const{
        bn_t e;
        bn_new(e);
        bn_set_dig(e, exponent);
        Zp r;
        bn_mxp(raw(r), raw(*this), e, order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::plus(const Zp& y) const{
        Zp r;
        bn_add(raw(r), raw(*this), raw(y));
        if (bn_cmp(raw(r), order<C>()) != RLC_LT) bn_sub(raw(r), raw(r), order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::times(const Zp& y) const{
        Zp r;
        bn_mul(raw(r), raw(*this), raw(y));
        bn_mod(raw(r), raw(r), order<C>());
        return r;
    }

    template <class C>
    Zp<C> Zp<C>::negated() const{
        if (is_zero()) return *this;
        Zp r;
        bn_sub(raw(r), order<C>(), raw(*this));
        return r;
    }

    template <class C>
    bool Zp<C>::equals(const Zp& y) const{
        return bn_cmp(raw(*this), raw(y)) == RLC_EQ;
    }

    template class Zp<detail::Tag>;
}
