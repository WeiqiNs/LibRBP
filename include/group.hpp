#pragma once

#include "field.hpp"
#include "gp.hpp"

using G1Vec = std::vector<G1>;
using G2Vec = std::vector<G2>;

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
     * 
     * @param x 
     * @return 
     */
    [[nodiscard]] static G1 g1_add_vec(const G1Vec& x);

    /**
     * 
     * @param x 
     * @return 
     */
    [[nodiscard]] static G2 g2_add_vec(const G2Vec& x);
    

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

private:
    /// Boolean to indicate whether precomputed table should be used.
    bool pre_table;

    /// Generate for group 1 and 2.
    G1 gen_1;
    G2 gen_2;

    /// Precomputed tables for group 1 and 2.
    g1_t* table_1;
    g2_t* table_2;
};
