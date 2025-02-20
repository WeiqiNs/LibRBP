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
    /**
     * Constructor of the Zp field.
     * @param order the order of the group as Fp.
     */
    explicit Field(const Fp& order);

    /**
     * Constructor of the Zp field.
     * @param order the order of the group as integer.
     */
    explicit Field(const int& order);

    /// Compare two filed elements.
    static bool cmp(const Fp& x, const Fp& y);

    /// Compare a filed element with an integer.
    static bool cmp(const Fp& x, const int& y);

    /// Return a field element holding a one.
    static Fp one();

    /// Return a field element holding a zero.
    static Fp zero();

    /// Mod with the order of the group.
    void mod(Fp& x) const;

    /// Return a field element created from the input integer.
    [[nodiscard]] Fp from_int(const int& x) const;

    /// Return a field element holding a random value in the field.
    [[nodiscard]] Fp rand() const;

    /// Find x+y.
    [[nodiscard]] Fp add(const Fp& x, const Fp& y) const;

    /// Find x-y.
    [[nodiscard]] Fp sub(const Fp& x, const Fp& y) const;

    /// Find x*y.
    [[nodiscard]] Fp mul(const Fp& x, const Fp& y) const;

    /// Find x^y.
    [[nodiscard]] Fp exp(const Fp& x, const Fp& y) const;

    /// Find -x.
    [[nodiscard]] Fp neg(const Fp& x) const;

    /// Find x^(-1).
    [[nodiscard]] Fp inv(const Fp& x) const;

    /// Mod vector with the order of the group.
    void mod(FpVec& x) const;

    /// Find x||y.
    static FpVec vec_join(const FpVec& x, const FpVec& y);

    /// Create a vector of field elements from a vector of integers.
    [[nodiscard]] FpVec from_int(const IntVec& x) const;

    /// Create a vector of random field elements.
    [[nodiscard]] FpVec rand_vec(const int& size) const;

    /// Find x+y.
    [[nodiscard]] FpVec vec_add(const FpVec& x, const FpVec& y) const;

    /// Find x-y.
    [[nodiscard]] FpVec vec_sub(const FpVec& x, const FpVec& y) const;

    /// Find x*y, the hadamard product.
    [[nodiscard]] FpVec vec_mul(const FpVec& x, const FpVec& y) const;

    /// Find x*y, where y is some constant.
    [[nodiscard]] FpVec vec_mul(const FpVec& x, const Fp& y) const;

    /// Find x^(-1).
    [[nodiscard]] FpVec vec_inv(const FpVec& x) const;

    /// Find sum(x).
    [[nodiscard]] Fp vec_sum(const FpVec& x) const;

    /// Find <x,y>.
    [[nodiscard]] Fp vec_ip(const FpVec& x, const FpVec& y) const;

    /// Interpolate the polynomial given roots and degree, the coefficients are returned in format c + a0 + a1 + ...
    [[nodiscard]] FpVec poly_interpolate(const int& degree, const FpVec& roots) const;

    /// Mod matrix with the order of the group.
    void mod(FpMat& x) const;

    /// Determine if a matrix is the identity matrix.
    static bool mat_is_id(const FpMat& x);

    /// Create an identity matrix of size by size.
    static FpMat mat_id(const int& size);

    /// Find the transpose of the input matrix.
    static FpMat mat_transpose(const FpMat& x);

    /// Concatenate two input matrices.
    static FpMat mat_join(const FpMat& x, const FpMat& y);

    /// Create a matrix of field elements from integers.
    [[nodiscard]] FpMat from_int(const IntMat& x) const;

    /// Create a matrix of random field elements.
    [[nodiscard]] FpMat rand_mat(const int& row, const int& col) const;

    /// Find x*y, where x is a matrix and y is a vector.
    [[nodiscard]] FpVec mat_mul(const FpMat& x, const FpVec& y) const;

    /// Find x*y, where x is a vector and y is a matrix.
    [[nodiscard]] FpVec mat_mul(const FpVec& x, const FpMat& y) const;

    /// Find x*y, where both x and y are matrices.
    [[nodiscard]] FpMat mat_mul(const FpMat& x, const FpMat& y) const;

    /// Find x*y, where x is a matrix and y is some constant.
    [[nodiscard]] FpMat mat_mul(const FpMat& x, const Fp& y) const;

    /// Find the inverse of input matrix x.
    [[nodiscard]] FpMat mat_inv(const FpMat& x) const;

    /// Find the inverse of input matrix x, as well as the determinant of x.
    [[nodiscard]] FpMat mat_inv_with_det(const FpMat& x, Fp& det) const;

private:
    /// Store the order of the group.
    Fp prime;
};
