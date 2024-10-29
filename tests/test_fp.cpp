#include <gtest/gtest.h>
#include "bp.hpp"

TEST(FieldTests, FieldPoint){
    // Initialize the scheme.
    BP::init();

    // Create a vector of length 10.
    std::vector<Fp> x;
    x.resize(10);

    // Create a new field point.
    Fp y;
    bn_set_dig(y.value, 10);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test and then clear the core.
    EXPECT_EQ(bn_cmp_dig(x.back().value, 10), RLC_EQ);
    EXPECT_TRUE(bn_is_zero(x[0].value));
    BP::close();
}
