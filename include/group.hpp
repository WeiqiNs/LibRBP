#pragma once

#include "field.hpp"
#include "gp.hpp"

using G1Vec = std::vector<G1>;
using G2Vec = std::vector<G2>;
using G1Mat = std::vector<G1Vec>;
using G2Mat = std::vector<G2Vec>;
using Bytes = std::vector<uint8_t>;
using ByteVec = std::vector<Bytes>;

class Group{
public:
    /**
     * Default generator for creating the bilinear pairing group.
     * By default, it samples generators for the source groups G1 and G2.
     * @param pre a boolean indicating whether using precompute tables for group exponentiation.
     */
    explicit Group(const bool& pre = true);

    /**
     * Compare if two Gt elements are the same.
     * @param x an element in Gt.
     * @param y an element in Gt.
     * @return a boolean indicating whether the two elements are the same.
     */
    static bool cmp_gt(const Gt& x, const Gt& y);

    /**
     * Check if an element in Gt is the unity, i.e. 0.
     * @param x an element in Gt.
     * @return a boolean indicating whether the input element is the unity.
     */
    static bool check_gt_unity(const Gt& x);

    /**
     * Perform pairing on group elements.
     * @param x an element in G1.
     * @param y an element in G2.
     * @return an element in Gt.
     */
    [[nodiscard]] static Gt pair(const G1& x, const G2& y);

    /**
     * Perform pairing on vectors of group elements and find their products.
     * @param x a vector of elements in G1.
     * @param y a vector of elements in G2.
     * @return an element in Gt.
     */
    [[nodiscard]] static Gt pair(const G1Vec& x, const G2Vec& y);

    /**
     * Given a vector of G1 elements, add them (multiply to add the exponent.)
     * @param x a vector of elements in G1.
     * @return an element in G1.
     */
    [[nodiscard]] static G1 g1_add_vec(const G1Vec& x);

    /**
     * Given a vector of G2 elements, add them (multiply to add the exponent.)
     * @param x a vector of elements in G2.
     * @return an element in G2.
     */
    [[nodiscard]] static G2 g2_add_vec(const G2Vec& x);

    /**
     * Given a Gt element, raise it to an integer.
     * @param x an element in Gt.
     * @param y an integer.
     * @return an element in Gt.
     */
    [[nodiscard]] Gt static gt_raise(const Gt& x, const int& y);

    /**
     * Given a Gt element, raise it to an integer.
     * @param x an element in Gt.
     * @param y a field point.
     * @return an element in Gt.
     */
    [[nodiscard]] Gt static gt_raise(const Gt& x, const Fp& y);

    /// Get the gt element with subgroup generators.
    [[nodiscard]] Gt get_gt() const;

    /**
     * Raise the generator G1 to some Zp value.
     * @param x a field point.
     * @return an element in G1.
     */
    [[nodiscard]] G1 g1_raise(const Fp& x) const;

    /**
     * Raise the generator G2 to some Zp value.
     * @param x a field point.
     * @return an element in G2.
     */
    [[nodiscard]] G2 g2_raise(const Fp& x) const;

    /**
     * Raise the target group element to an integer.
     * @param x an integer.
     * @return an element in Gt.
     */
    [[nodiscard]] Gt gt_raise(const int& x) const;

    /**
     * Raise the target group element to an integer.
     * @param x a field point.
     * @return an element in Gt.
     */
    [[nodiscard]] Gt gt_raise(const Fp& x) const;

    /**
     * Raise the generator G1 to a vector of Zp values.
     * @param x a vector of field points.
     * @return a vector of elements in G1.
     */
    [[nodiscard]] G1Vec g1_raise(const FpVec& x) const;

    /**
     * Raise the generator G2 to a vector of Zp values.
     * @param x a vector of field points.
     * @return a vector of elements in G2.
     */
    [[nodiscard]] G2Vec g2_raise(const FpVec& x) const;

    /// Dump a G1 element to bytes.
    [[nodiscard]] Bytes g1_dump(const G1& x) const;

    /// Dump a G2 element to bytes.
    [[nodiscard]] Bytes g2_dump(const G2& x) const;

    /// Dump a vector of G1 element to a vector of bytes.
    [[nodiscard]] ByteVec g1_dump(const G1Vec& x) const;

    /// Dump a vector of G2 element to a vector of bytes.
    [[nodiscard]] ByteVec g2_dump(const G2Vec& x) const;

    /// Load a G1 element from bytes.
    [[nodiscard]] G1 g1_load(const Bytes& x) const;

    /// Load a G2 element from bytes.
    [[nodiscard]] G2 g2_load(const Bytes& x) const;

    /// Load a vector of G1 element from a vector of bytes.
    [[nodiscard]] G1Vec g1_load(const ByteVec& x) const;

    /// Load a vector of G2 element from a vector of bytes.
    [[nodiscard]] G2Vec g2_load(const ByteVec& x) const;

private:
    /// Boolean to indicate whether precomputed table should be used.
    bool pre_table;

    /// Generate for group 1 and 2.
    G1 gen_1;
    G2 gen_2;

    /// Generator in the target group.
    Gt gt;

    /// Sizes for g1 and g2 elements.
    int g1_size;
    int g2_size;

    /// Precomputed tables for group 1 and 2.
    g1_t* table_1;
    g2_t* table_2;
};
