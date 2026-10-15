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
    std::string_view known_hash;
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
