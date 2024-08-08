#pragma once

#include "field.hpp"
#include "gp.hpp"

using G1Vec = std::vector<G1>;
using G2Vec = std::vector<G2>;

class Group{
public:
    explicit Group(const bool& pre = true);

    static bool cmp_gt(const Gt& x, const Gt& y);

    static Gt pair(const G1& x, const G2& y);

    static Gt pair(const G1Vec& x, const G2Vec& y);

    [[nodiscard]] G1 g1_raise(const Fp& x) const;

    [[nodiscard]] G2 g2_raise(const Fp& x) const;

    [[nodiscard]] G1Vec g1_raise(const FpVec& x) const;

    [[nodiscard]] G2Vec g2_raise(const FpVec& x) const;

private:
    bool pre_table;

    G1 gen_1;

    G2 gen_2;

    g1_t* table_1;

    g2_t* table_2;
};
