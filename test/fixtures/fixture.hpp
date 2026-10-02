#ifndef RBP_TEST_FIXTURE_HPP
#define RBP_TEST_FIXTURE_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <rbp/core.hpp>

struct PointFixture{
    std::size_t compressed_size;
    std::optional<std::uint8_t> off_subgroup_x;

    [[nodiscard]] std::optional<rbp::Bytes> off_subgroup() const{
        if (!off_subgroup_x) return std::nullopt;
        rbp::Bytes out(compressed_size, 0);
        out.front() = 0x02;
        out.back() = *off_subgroup_x;
        return out;
    }
};

template <class C>
struct Fixture;

template <class P>
struct PointTraits;

template <class C, rbp::Side S>
struct PointTraits<rbp::Point<C, S>>{
    using Curve = C;

    static constexpr PointFixture fixture = S == rbp::Side::g1 ? Fixture<C>::g1 : Fixture<C>::g2;
};

#endif
