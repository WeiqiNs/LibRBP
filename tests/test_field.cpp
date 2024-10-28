#include "bp.hpp"
#include <gtest/gtest.h>

TEST(FieldElementTests, Add){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.add(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 2));
    BP::close();
}

TEST(FieldElementTests, Sub){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.sub(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 8));
    BP::close();
}

TEST(FieldElementTests, Mul){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.mul(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 7));
    BP::close();
}

TEST(FieldElementTests, Exp){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.exp(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 4));
    BP::close();
}

TEST(FieldElementTests, Neg){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const Fp x = Zp.from_int(5);
    const Fp r = Zp.neg(x);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 6));
    BP::close();
}

TEST(FieldElementTests, Inv){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const Fp x = Zp.from_int(5);
    const Fp r = Zp.inv(x);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 9));
    BP::close();
}

TEST(FieldElementTests, VecJoin){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Field::vec_join(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 1) && Field::cmp(r[9], 10));
    BP::close();
}

TEST(FieldElementTests, VecAdd){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Zp.vec_add(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 7) && Field::cmp(r[4], 4));
    BP::close();
}

TEST(FieldElementTests, VecSub){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Zp.vec_sub(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 6) && Field::cmp(r[4], 6));
    BP::close();
}

TEST(FieldElementTests, VecMul){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Zp.vec_mul(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 6) && Field::cmp(r[4], 6));
    BP::close();
}

TEST(FieldElementTests, VecMulInt){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const Fp y = Zp.from_int(8);
    const FpVec r = Zp.vec_mul(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 8) && Field::cmp(r[4], 7));
    BP::close();
}

TEST(FieldElementTests, VecInv){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec r = Zp.vec_inv(x);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 1) && Field::cmp(r[4], 9));
    BP::close();
}

TEST(FieldElementTests, VecIp){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const Fp r = Zp.vec_ip(x, y);

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r, 9));
    BP::close();
}

TEST(FieldElementTests, VecPolyInterpolate){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpVec x = Zp.from_int(IntVec{2, 4, 6, 8, 10});
    const FpVec r = Zp.poly_interpolate(10, x);

    // Test and then clear the core.
    EXPECT_TRUE(
        r.size() == 11 &&
        Field::cmp(r[0], 10) &&
        Field::cmp(r[1], 6) &&
        Field::cmp(r[2], 4) &&
        Field::cmp(r[3], 10) &&
        Field::cmp(r[4], 3) &&
        Field::cmp(r[5], 1) &&
        Field::cmp(r[6], 0) &&
        Field::cmp(r[10], 0)
    );
    BP::close();
}

TEST(FieldElementTests, MatId){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpMat x = Zp.from_int({{0, 1, 2}, {2, 3, 4}, {3, 4, 5}});

    // Test and then clear the core.
    EXPECT_TRUE(Field::mat_is_id(Field::mat_id(100)) && !Field::mat_is_id(x));
    BP::close();
}

TEST(FieldElementTests, MatTranspose){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const FpMat r = Field::mat_transpose(Zp.from_int(x));

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0][0], 1) && Field::cmp(r[4][2], 4));
    BP::close();
}

TEST(FieldElementTests, MatJoin){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const IntMat y = {{11, 12, 13, 14, 15}, {6, 7, 8, 9, 10}, {1, 2, 3, 4, 5}};
    const FpMat zp_rp = Field::mat_join(Zp.from_int(x), Zp.from_int(y));

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(zp_rp[2][0], 0) && Field::cmp(zp_rp[2][9], 5));
    BP::close();
}

TEST(FieldElementTests, MatMulVec){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const IntVec y = {1, 2, 3, 4, 5};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(y));

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 0) && Field::cmp(r[1], 9) && Field::cmp(r[2], 7));
    BP::close();
}

TEST(FieldElementTests, VecMulMat){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const IntVec x = {1, 2, 3};
    const IntMat y = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(y));

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0], 2) && Field::cmp(r[2], 3) && Field::cmp(r[4], 4));
    BP::close();
}

TEST(FieldElementTests, MatMulMat){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}};
    const IntMat y = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(y));

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0][0], 7) && Field::cmp(r[1][1], 7));
    BP::close();
}

TEST(FieldElementTests, MatMulInv){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(5));

    // Test and then clear the core.
    EXPECT_TRUE(Field::cmp(r[0][0], 5) && Field::cmp(r[2][4], 9));
    BP::close();
}

TEST(FieldElementTests, MatInv){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    const FpMat x = Zp.from_int({{3, 5, 8}, {2, 2, 2}, {9, 9, 3}});
    const FpMat xi = Zp.mat_inv(x);

    // Test and then clear the core.
    EXPECT_TRUE(Field::mat_is_id(Zp.mat_mul(x, xi)));
    BP::close();
}

TEST(FieldElementTests, MatInvDet){
    // Initialize the scheme.
    BP::init();

    // Create a field.
    const Field Zp(11);

    // Perform testing.
    Fp det;
    const FpMat x = Zp.from_int({{3, 5, 8}, {2, 2, 2}, {9, 9, 3}});
    const FpMat xi = Zp.mat_inv_with_det(x, det);

    // Test and then clear the core.
    EXPECT_TRUE(Field::mat_is_id(Zp.mat_mul(x, xi)) && Field::cmp(det, 2));
    BP::close();
}
