#pragma once

#include <algorithm>
#include <vector>
#include <stdexcept>
#include "fp.hpp"

// Declare some alias for commonly used types.
using FpVec = std::vector<Fp>;
using FpMat = std::vector<FpVec>;
using IntVec = std::vector<int>;
using IntMat = std::vector<IntVec>;

class Field{
public:
    Field() = default;

    explicit Field(const Fp& order);

    explicit Field(const int& order);

    void set_order(const Fp& order);

    void set_order(const int& order);

    static bool cmp(const Fp& x, const Fp& y);

    static bool cmp(const Fp& x, const int& y);

    static Fp one();

    static Fp zero();

    void mod(Fp& x) const;

    [[nodiscard]] Fp from_int(const int& x) const;

    [[nodiscard]] Fp rand() const;

    [[nodiscard]] Fp add(const Fp& x, const Fp& y) const;

    [[nodiscard]] Fp sub(const Fp& x, const Fp& y) const;

    [[nodiscard]] Fp mul(const Fp& x, const Fp& y) const;

    [[nodiscard]] Fp exp(const Fp& x, const Fp& y) const;

    [[nodiscard]] Fp neg(const Fp& x) const;

    [[nodiscard]] Fp inv(const Fp& x) const;

    static FpVec vec_join(const FpVec& x, const FpVec& y);

    [[nodiscard]] FpVec from_int(const IntVec& x) const;

    [[nodiscard]] FpVec rand_vec(const int& size) const;

    [[nodiscard]] FpVec vec_add(const FpVec& x, const FpVec& y) const;

    [[nodiscard]] FpVec vec_sub(const FpVec& x, const FpVec& y) const;

    [[nodiscard]] FpVec vec_mul(const FpVec& x, const FpVec& y) const;

    [[nodiscard]] FpVec vec_mul(const FpVec& x, const Fp& y) const;

    [[nodiscard]] FpVec vec_inv(const FpVec& x) const;

    [[nodiscard]] Fp vec_ip(const FpVec& x, const FpVec& y) const;

    [[nodiscard]] FpVec poly_interpolate(const int& degree, const FpVec& roots) const;

    static bool mat_is_id(const FpMat& x);

    static FpMat mat_id(const int& size);

    static FpMat mat_transpose(const FpMat& x);

    static FpMat mat_join(const FpMat& x, const FpMat& y);

    [[nodiscard]] FpMat from_int(const IntMat& x) const;

    [[nodiscard]] FpMat rand_mat(const int& row, const int& col) const;

    [[nodiscard]] FpVec mat_mul(const FpMat& x, const FpVec& y) const;

    [[nodiscard]] FpVec mat_mul(const FpVec& x, const FpMat& y) const;

    [[nodiscard]] FpMat mat_mul(const FpMat& x, const FpMat& y) const;

    [[nodiscard]] FpMat mat_mul(const FpMat& x, const Fp& y) const;

    [[nodiscard]] FpMat mat_inv(const FpMat& x) const;

    [[nodiscard]] FpMat mat_inv_with_det(const FpMat& x, Fp& det) const;

private:
    Fp prime;
};
