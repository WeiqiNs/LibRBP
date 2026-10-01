#pragma once

#include "fixture.hpp"
#include "rbp/bn254.hpp"

template <>
struct Fixture<rbp::BN254>{
    static constexpr std::string_view order = "16798108731015832284940804142231733909759579603404752749028378864165570215949";
    static constexpr PointFixture g1{33, std::nullopt};
    static constexpr PointFixture g2{65, 2};
};
