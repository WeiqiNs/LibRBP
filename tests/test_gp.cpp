#include "bp.hpp"
#include <gtest/gtest.h>

TEST(GroupElementTests, G1Element){
    // Initialize the scheme.
    BP::init();

    // Create a vector of length 10.
    std::vector<G1> x;
    x.resize(10);

    // Create a new field point.
    G1 y;
    g1_rand(y.value);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test and then clear the core.
    EXPECT_EQ(g1_cmp(x.back().value, y.value), RLC_EQ);
    EXPECT_TRUE(g1_is_infty(x[0].value));
    BP::close();
}

TEST(GroupElementTests, G2Element){
    // Initialize the scheme.
    BP::init();

    // Create a vector of length 10.
    std::vector<G2> x;
    x.resize(10);

    // Create a new field point.
    G2 y;
    g2_rand(y.value);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test and then clear the core.
    EXPECT_EQ(g2_cmp(x.back().value, y.value), RLC_EQ);
    EXPECT_TRUE(g2_is_infty(x[0].value));
    BP::close();
}

TEST(GroupElementTests, GtElement){
    // Initialize the scheme.
    BP::init();

    // Create a vector of length 10.
    std::vector<Gt> x;
    x.resize(10);

    // Create a new field point.
    Gt y;
    gt_rand(y.value);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test and then clear the core.
    EXPECT_EQ(gt_cmp(x.back().value, y.value), RLC_EQ);
    EXPECT_TRUE(gt_is_unity(x[0].value));
    BP::close();
}
