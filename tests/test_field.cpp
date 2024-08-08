#include "field.hpp"

bool test_add(const Field& Zp){
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.add(x, y);
    return Field::cmp(r, 2);
}

bool test_sub(const Field& Zp){
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.sub(x, y);
    return Field::cmp(r, 8);
}

bool test_mul(const Field& Zp){
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.mul(x, y);
    return Field::cmp(r, 7);
}

bool test_exp(const Field& Zp){
    const Fp x = Zp.from_int(5);
    const Fp y = Zp.from_int(8);
    const Fp r = Zp.exp(x, y);
    return Field::cmp(r, 4);
}

bool test_neg(const Field& Zp){
    const Fp x = Zp.from_int(5);
    const Fp r = Zp.neg(x);
    return Field::cmp(r, 6);
}

bool test_inv(const Field& Zp){
    const Fp x = Zp.from_int(5);
    const Fp r = Zp.inv(x);
    return Field::cmp(r, 9);
}

bool test_join_vec(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Field::vec_join(x, y);
    return Field::cmp(r[0], 1) && Field::cmp(r[9], 10);
}

bool test_add_vec(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Zp.vec_add(x, y);
    return Field::cmp(r[0], 7) && Field::cmp(r[4], 4);
}

bool test_sub_vec(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Zp.vec_sub(x, y);
    return Field::cmp(r[0], 6) && Field::cmp(r[4], 6);
}

bool test_mul_vec(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const FpVec r = Zp.vec_mul(x, y);
    return Field::cmp(r[0], 6) && Field::cmp(r[4], 6);
}

bool test_mul_vec_int(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const Fp y = Zp.from_int(8);
    const FpVec r = Zp.vec_mul(x, y);
    return Field::cmp(r[0], 8) && Field::cmp(r[4], 7);
}

bool test_inv_vec(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec r = Zp.vec_inv(x);
    return Field::cmp(r[0], 1) && Field::cmp(r[4], 9);
}

bool test_ip_vec(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{1, 2, 3, 4, 5});
    const FpVec y = Zp.from_int(IntVec{6, 7, 8, 9, 10});
    const Fp r = Zp.vec_ip(x, y);
    return Field::cmp(r, 9);
}

bool test_poly_interpolate(const Field& Zp){
    const FpVec x = Zp.from_int(IntVec{2, 4, 6, 8, 10});
    const FpVec r = Zp.poly_interpolate(10, x);
    return r.size() == 11 &&
        Field::cmp(r[0], 10) &&
        Field::cmp(r[1], 6) &&
        Field::cmp(r[2], 4) &&
        Field::cmp(r[3], 10) &&
        Field::cmp(r[4], 3) &&
        Field::cmp(r[5], 1) &&
        Field::cmp(r[6], 0) &&
        Field::cmp(r[10], 0);
}

bool test_id_mat(const Field& Zp){
    const FpMat x = Zp.from_int({{0, 1, 2}, {2, 3, 4}, {3, 4, 5}});
    return Field::mat_is_id(Field::mat_id(100)) && !Field::mat_is_id(x);
}

bool test_transpose_mat(const Field& Zp){
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const FpMat r = Field::mat_transpose(Zp.from_int(x));

    return Field::cmp(r[0][0], 1) && Field::cmp(r[4][2], 4);
}

bool test_join_mat(const Field& Zp){
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const IntMat y = {{11, 12, 13, 14, 15}, {6, 7, 8, 9, 10}, {1, 2, 3, 4, 5}};
    const FpMat zp_rp = Field::mat_join(Zp.from_int(x), Zp.from_int(y));

    return Field::cmp(zp_rp[2][0], 0) && Field::cmp(zp_rp[2][9], 5);
}

bool test_mul_mat_vec(const Field& Zp){
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const IntVec y = {1, 2, 3, 4, 5};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(y));
    return Field::cmp(r[0], 0) && Field::cmp(r[1], 9) && Field::cmp(r[2], 7);
}

bool test_mul_vec_mat(const Field& Zp){
    const IntVec x = {1, 2, 3};
    const IntMat y = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(y));
    return Field::cmp(r[0], 2) && Field::cmp(r[2], 3) && Field::cmp(r[4], 4);
}

bool test_mul_mat_mat(const Field& Zp){
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}};
    const IntMat y = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(y));
    return Field::cmp(r[0][0], 7) && Field::cmp(r[1][1], 7);
}

bool test_mul_mat_int(const Field& Zp){
    const IntMat x = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    const auto r = Zp.mat_mul(Zp.from_int(x), Zp.from_int(5));
    return Field::cmp(r[0][0], 5) && Field::cmp(r[2][4], 9);
}

bool test_mul_inv(const Field& Zp){
    const FpMat x = Zp.from_int({{3, 5, 8}, {2, 2, 2}, {9, 9, 3}});
    const FpMat xi = Zp.mat_inv(x);
    return Field::mat_is_id(Zp.mat_mul(x, xi));
}

bool test_mul_inv_with_det(const Field& Zp){
    Fp det;
    const FpMat x = Zp.from_int({{3, 5, 8}, {2, 2, 2}, {9, 9, 3}});
    const FpMat xi = Zp.mat_inv_with_det(x, det);
    return Field::mat_is_id(Zp.mat_mul(x, xi)) && Field::cmp(det, 2);
}

int main(){
    // Initialize the relic core.
    core_init();

    // Create a field.
    const Field Zp(11);

    // Perform integer tests.
    if (test_add(Zp) != true) return 1;
    if (test_sub(Zp) != true) return 1;
    if (test_mul(Zp) != true) return 1;
    if (test_exp(Zp) != true) return 1;
    if (test_neg(Zp) != true) return 1;
    if (test_inv(Zp) != true) return 1;

    // Perform vector tests.
    if (test_join_vec(Zp) != true) return 1;
    if (test_add_vec(Zp) != true) return 1;
    if (test_sub_vec(Zp) != true) return 1;
    if (test_mul_vec(Zp) != true) return 1;
    if (test_mul_vec_int(Zp) != true) return 1;
    if (test_inv_vec(Zp) != true) return 1;
    if (test_ip_vec(Zp) != true) return 1;
    if (test_poly_interpolate(Zp) != true) return 1;

    // Perform matrix tests.
    if (test_id_mat(Zp) != true) return 1;
    if (test_transpose_mat(Zp) != true) return 1;
    if (test_join_mat(Zp) != true) return 1;
    if (test_mul_mat_vec(Zp) != true) return 1;
    if (test_mul_vec_mat(Zp) != true) return 1;
    if (test_mul_mat_mat(Zp) != true) return 1;
    if (test_mul_mat_int(Zp) != true) return 1;
    if (test_mul_inv(Zp) != true) return 1;
    if (test_mul_inv_with_det(Zp) != true) return 1;

    // Clean the relic core.
    core_clean();

    return 0;
}
