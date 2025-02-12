#include "group.hpp"

Group::Group(const bool& pre){
    // The constructor only needs to sample group generator.
    g1_get_gen(gen_1.value);
    g2_get_gen(gen_2.value);

    // If we use the precomputed table.
    if (pre){
        // Table for group 1.
        table_1 = static_cast<g1_t*>(malloc(sizeof(g1_t) * RLC_G1_TABLE));
        g1_mul_pre(table_1, gen_1.value);

        // Table for group 2.
        table_2 = static_cast<g2_t*>(malloc(sizeof(g2_t) * RLC_G2_TABLE));
        g2_mul_pre(table_2, gen_2.value);

        pre_table = true;
    }
    else{
        pre_table = false;
    }
}

bool Group::cmp_gt(const Gt& x, const Gt& y){
    return gt_cmp(x.value, y.value) == RLC_EQ;
}

bool Group::check_gt_unity(const Gt& x){
    return gt_is_unity(x.value);
}

Gt Group::pair(const G1& x, const G2& y){
    Gt r;
    pc_map(r.value, x.value, y.value);
    return r;
}

Gt Group::pair(const G1Vec& x, const G2Vec& y){
    // Check that inputs have to have equal length.
    if (x.size() != y.size()){
        throw std::runtime_error("The input vectors are of different lengths.");
    }

    const auto size = x.size();

    std::vector<g1_t> xv(x.size());
    for (int i = 0; i < size; ++i) g1_copy(xv[i], x[i].value);

    std::vector<g2_t> yv(y.size());
    for (int i = 0; i < size; ++i) g2_copy(yv[i], y[i].value);

    Gt r;
    pc_map_sim(r.value, xv.data(), yv.data(), size);

    return r;
}

G1 Group::g1_add_vec(const G1Vec& x){
    G1 r = x[0];

    for (int i = 1; i < x.size(); ++i) g1_add(r.value, r.value, x[i].value);

    return r;
}

G2 Group::g2_add_vec(const G2Vec& x){
    G2 r = x[0];

    for (int i = 1; i < x.size(); ++i) g2_add(r.value, r.value, x[i].value);

    return r;
}

G1 Group::g1_raise(const Fp& x) const{
    G1 r;

    if (pre_table) g1_mul_fix(r.value, table_1, x.value);
    else g1_mul(r.value, gen_1.value, x.value);

    return r;
}

G2 Group::g2_raise(const Fp& x) const{
    G2 r;

    if (pre_table) g2_mul_fix(r.value, table_2, x.value);
    else g2_mul(r.value, gen_2.value, x.value);

    return r;
}

G1Vec Group::g1_raise(const FpVec& x) const{
    G1Vec r;

    for (const auto& i : x) r.push_back(g1_raise(i));

    return r;
}

G2Vec Group::g2_raise(const FpVec& x) const{
    G2Vec r;

    for (const auto& i : x) r.push_back(g2_raise(i));

    return r;
}
