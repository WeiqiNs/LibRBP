#ifndef RBP_TEST_FIXTURE_BN254_HPP
#define RBP_TEST_FIXTURE_BN254_HPP

#include "fixture.hpp"
#include <rbp/bn254.hpp>

template <>
struct Fixture<rbp::BN254>{
    static constexpr std::string_view order = "16798108731015832284940804142231733909759579603404752749028378864165570215949";
    static constexpr std::string_view known_zp_hash = "11893060188359645458589135973354661239534122605687624716887070145979496602170";
    static constexpr PointFixture g1{33, std::nullopt,
        "0201be66e7b9b4cae96448142966413bfeb5191b32cc71e515159d87bbb697faf2"
    };
    static constexpr PointFixture g2{65, 2,
        "03232cecf2c6a441aa779b82d0e26fb9406d49fbaabd267f99ffad6de40f0e79b22342d5be22a5555742e574341abc64e1f2e0543f0d695bffa2bf38a5d4ffefcf"
    };
};

#endif
