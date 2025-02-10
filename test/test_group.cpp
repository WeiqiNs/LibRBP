#include <gtest/gtest.h>
#include "bp.hpp"

TEST(GroupTests, Pairing){
    // Initialize the scheme.
    BP::init();

    // Set testing points.
    Fp x, y;
    bn_set_dig(x.value, 11);
    bn_set_dig(y.value, 13);

    // Test pairing with precomputed table.
    const auto gp = Group();
    const auto r1 = Group::pair(gp.g1_raise(x), gp.g2_raise(y));
    const auto r2 = Group::pair(gp.g1_raise(y), gp.g2_raise(x));

    // Test and then clear the core.
    EXPECT_TRUE(Group::cmp_gt(r1, r2));
    BP::close();
}

TEST(GroupTests, PairingNoPre){
    // Initialize the scheme.
    BP::init();

    // Set testing points.
    Fp x, y;
    bn_set_dig(x.value, 11);
    bn_set_dig(y.value, 13);

    // Test pairing without precomputed table.
    const auto gp = Group(false);
    const auto r1 = Group::pair(gp.g1_raise(x), gp.g2_raise(y));
    const auto r2 = Group::pair(gp.g1_raise(y), gp.g2_raise(x));

    // Test and then clear the core.
    EXPECT_TRUE(Group::cmp_gt(r1, r2));
    BP::close();
}

TEST(GroupTests, PairingVec){
    // Initialize the scheme.
    BP::init();

    // Set testing points.
    FpVec x(3), y(3);
    bn_set_dig(x[0].value, 11);
    bn_set_dig(x[1].value, 12);
    bn_set_dig(x[2].value, 13);
    bn_set_dig(y[0].value, 14);
    bn_set_dig(y[1].value, 15);
    bn_set_dig(y[2].value, 16);

    // Test pairing with vectors.
    const auto gp = Group(false);
    const auto r1 = Group::pair(gp.g1_raise(x), gp.g2_raise(y));
    const auto r2 = Group::pair(gp.g1_raise(y), gp.g2_raise(x));

    // Test and then clear the core.
    EXPECT_TRUE(Group::cmp_gt(r1, r2));
    BP::close();
}
