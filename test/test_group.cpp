#include <gtest/gtest.h>
#include "bp.hpp"

TEST(GroupTests, G1Load){
    // Initialize the scheme.
    BP::init();

    // Create the group.
    const auto gp = Group();

    // Create a random g1, dump and load g1.
    G1 x;
    g1_rand(x.value);
    const G1 y = gp.g1_load(gp.g1_dump(x));

    EXPECT_EQ(g1_cmp(x.value, y.value), RLC_EQ);
    BP::close();
}

TEST(GroupTests, G1LoadVec){
    // Initialize the scheme.
    BP::init();

    // Create the group.
    const auto gp = Group();

    // Create a vector of random g1, dump and load the vector.
    G1Vec x(5);
    for (auto& i : x){ g1_rand(i.value); }
    const G1Vec y = gp.g1_load(gp.g1_dump(x));

    for (int i = 0; i < x.size(); ++i)
        EXPECT_EQ(g1_cmp(x[i].value, y[i].value), RLC_EQ);
    BP::close();
}

TEST(GroupTests, G2Load){
    // Initialize the scheme.
    BP::init();

    // Create the group.
    const auto gp = Group();

    // Create a random g2, dump and load g2.
    G2 x;
    g2_rand(x.value);
    const G2 y = gp.g2_load(gp.g2_dump(x));

    EXPECT_EQ(g2_cmp(x.value, y.value), RLC_EQ);
    BP::close();
}

TEST(GroupTests, G2LoadVec){
    // Initialize the scheme.
    BP::init();

    // Create the group.
    const auto gp = Group();

    // Create a vector of random g2, dump and load the vector.
    G2Vec x(5);
    for (auto& i : x){ g2_rand(i.value); }
    const G2Vec y = gp.g2_load(gp.g2_dump(x));

    for (int i = 0; i < x.size(); ++i)
        EXPECT_EQ(g2_cmp(x[i].value, y[i].value), RLC_EQ);
    BP::close();
}

TEST(GroupTests, G1Add){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(101);

    // Set testing points.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const Fp y(15);

    // Test add.
    const auto gp = Group();
    const auto r1 = Group::g1_add_vec(gp.g1_raise(x));
    const auto r2 = gp.g1_raise(y);

    EXPECT_EQ(g1_cmp(r1.value, r2.value), RLC_EQ);
    BP::close();
}

TEST(GroupTests, G2Add){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(101);

    // Set testing points.
    const FpVec x = Zp.from_int(IntVec{2, 3, 4, 5, 6});
    const Fp y(20);

    // Test add.
    const auto gp = Group();
    const auto r1 = Group::g2_add_vec(gp.g2_raise(x));
    const auto r2 = gp.g2_raise(y);

    EXPECT_EQ(g2_cmp(r1.value, r2.value), RLC_EQ);
    BP::close();
}

TEST(GroupTests, GtRaise){
    // Initialize the scheme.
    BP::init();

    // Create a field and a group.
    const Field Zp(101);
    const auto gp = Group();
    const Fp x(20);

    // Set testing points.
    const auto r1 = Group::gt_raise(gp.get_gt(), x);
    const auto r2 = gp.gt_raise(x);
    const auto r3 = Group::gt_raise(gp.get_gt(), 515);
    const auto r4 = gp.gt_raise(515);

    // Test and then clear the core.
    EXPECT_TRUE(Group::cmp_gt(r1, r2));
    EXPECT_TRUE(Group::cmp_gt(r3, r4));
    BP::close();
}

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
    const auto r2 = Group::gt_raise(gp.get_gt(), 143);

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
    const auto r2 = Group::gt_raise(gp.get_gt(), 143);

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
    const auto r2 = Group::gt_raise(gp.get_gt(), 542);

    // Test and then clear the core.
    EXPECT_TRUE(Group::cmp_gt(r1, r2));
    BP::close();
}
