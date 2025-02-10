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

TEST(FieldTests, InitInt){
    // Initialize the scheme.
    BP::init();

    // Create a vector of length 10.
    const Fp x(11);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(x, 11));
    BP::close();
}

TEST(FieldTests, InitStr){
    // Initialize the scheme.
    BP::init();

    // Create a vector of length 10.
    const Fp x("100");

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(x, 100));
    BP::close();
}

TEST(FieldTests, InitChar){
    // Initialize the scheme.
    BP::init();

    // Create a vector of unsigned char.
    const std::vector<unsigned char> x_vec = {0x12, 0x34};

    // Create a vector of length 10.
    const Fp x(x_vec);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(x, 4660));
    BP::close();
}
