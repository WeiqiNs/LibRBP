#include "group.hpp"

Group::Group(const bool& pre){
    // The constructor only needs to sample group generator.
    g1_get_gen(gen_1.value);
    g2_get_gen(gen_2.value);

    // Compute the generator.
    gt = pair(gen_1, gen_2);

    // Get the sizes of these values.
    g1_size = g1_size_bin(gen_1.value, 1);
    g2_size = g2_size_bin(gen_2.value, 1);

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
    for (int i = 0; i < size; ++i)
        g1_copy(xv[i], x[i].value);

    std::vector<g2_t> yv(y.size());
    for (int i = 0; i < size; ++i)
        g2_copy(yv[i], y[i].value);

    Gt r;
    pc_map_sim(r.value, xv.data(), yv.data(), size);

    return r;
}

G1 Group::g1_add_vec(const G1Vec& x){
    G1 r = x[0];

    for (int i = 1; i < x.size(); ++i)
    g1_add(r.value, r.value, x[i].value);

    return r;
}

G2 Group::g2_add_vec(const G2Vec& x){
    G2 r = x[0];

    for (int i = 1; i < x.size(); ++i)
    g2_add(r.value, r.value, x[i].value);

    return r;
}

Gt Group::gt_raise(const Gt& x, const int& y){
    Gt r;
    gt_exp_dig(r.value, x.value, y);
    return r;
}

Gt Group::gt_raise(const Gt& x, const Fp& y){
    Gt r;
    gt_exp(r.value, x.value, y.value);
    return r;
}

Gt Group::get_gt() const{ return gt; }

G1 Group::g1_raise(const Fp& x) const{
    G1 r;

    if (pre_table)
    g1_mul_fix(r.value, table_1, x.value);
    else g1_mul(r.value, gen_1.value, x.value);

    return r;
}

G2 Group::g2_raise(const Fp& x) const{
    G2 r;

    if (pre_table)
    g2_mul_fix(r.value, table_2, x.value);
    else g2_mul(r.value, gen_2.value, x.value);

    return r;
}

Gt Group::gt_raise(const int& x) const{ return gt_raise(gt, x); }

Gt Group::gt_raise(const Fp& x) const{ return gt_raise(gt, x); }

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

Bytes Group::g1_dump(const G1& x) const{
    Bytes r(g1_size);

    g1_write_bin(r.data(), g1_size, x.value, 1);

    return r;
}

Bytes Group::g2_dump(const G2& x) const{
    Bytes r(g2_size);

    g2_write_bin(r.data(), g2_size, x.value, 1);

    return r;
}

ByteVec Group::g1_dump(const G1Vec& x) const{
    ByteVec r(x.size());

    for (int i = 0; i < x.size(); ++i) r[i] = g1_dump(x[i]);

    return r;
}

ByteVec Group::g2_dump(const G2Vec& x) const{
    ByteVec r(x.size());

    for (int i = 0; i < x.size(); ++i) r[i] = g2_dump(x[i]);

    return r;
}

G1 Group::g1_load(const Bytes& x) const{
    G1 r;

    g1_read_bin(r.value, x.data(), g1_size);

    return r;
}

G2 Group::g2_load(const Bytes& x) const{
    G2 r;

    g2_read_bin(r.value, x.data(), g2_size);

    return r;
}

G1Vec Group::g1_load(const ByteVec& x) const{
    G1Vec r(x.size());

    for (int i = 0; i < x.size(); ++i) r[i] = g1_load(x[i]);

    return r;
}


G2Vec Group::g2_load(const ByteVec& x) const{
    G2Vec r(x.size());

    for (int i = 0; i < x.size(); ++i) r[i] = g2_load(x[i]);

    return r;
}
