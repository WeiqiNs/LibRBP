#include <vector>
#include "gp.hpp"

bool test_g1(){
    // Create a vector of length 10.
    std::vector<G1> x;
    x.resize(10);

    // Create a new field point.
    G1 y;
    g1_rand(y.value);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test assignment.
    return g1_cmp(x.back().value, y.value) == RLC_EQ && g1_is_infty(x[0].value);
}

bool test_g2(){
    // Create a vector of length 10.
    std::vector<G2> x;
    x.resize(10);

    // Create a new field point.
    G2 y;
    g2_rand(y.value);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test assignment.
    return g2_cmp(x.back().value, y.value) == RLC_EQ && g2_is_infty(x[0].value);
}

bool test_gt(){
    // Create a vector of length 10.
    std::vector<Gt> x;
    x.resize(10);

    // Create a new field point.
    Gt y;
    gt_rand(y.value);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test assignment.
    return gt_cmp(x.back().value, y.value) == RLC_EQ && gt_is_unity(x[0].value);
}

int main(){
    // Initialize the relic core.
    core_init();
    pc_param_set_any();

    // Perform tests.
    if (test_g1() != true) return 1;
    if (test_g2() != true) return 1;
    if (test_gt() != true) return 1;

    // Clean the relic core.
    core_clean();

    return 0;
}