#pragma once

#include "fixture.hpp"
#include "rbp/bls12_381.hpp"

template <>
struct Fixture<rbp::BLS12_381>{
    static constexpr std::string_view order = "52435875175126190479447740508185965837690552500527637822603658699938581184513";
    static constexpr PointFixture g1{49, 4};
    static constexpr PointFixture g2{97, 1};
};
