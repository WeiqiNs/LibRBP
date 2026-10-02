#ifndef RBP_TEST_FIXTURE_SS1536_HPP
#define RBP_TEST_FIXTURE_SS1536_HPP

#include "fixture.hpp"
#include <rbp/ss1536.hpp>

template <>
struct Fixture<rbp::SS1536>{
    static constexpr std::string_view order = "57896044618658097711785492504343953926634992332820282019728792006155588075521";
    static constexpr PointFixture g1{193, 1};
    static constexpr PointFixture g2{193, 1};
};

#endif
