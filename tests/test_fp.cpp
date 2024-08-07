#include <vector>
#include "fp.hpp"

bool test_fp(){
    // Create a vector of length 10.
    std::vector<Fp> x;
    x.resize(10);

    // Create a new field point.
    Fp y;
    bn_set_dig(y.value, 10);

    // Operator assignment.
    x[x.size() - 1] = y;

    // Test assignment.
    return bn_cmp_dig(x.back().value, 10) == RLC_EQ && bn_is_zero(x[0].value);
}

int main(){
    // Initialize the relic core.
    core_init();

    // Perform tests.
    if (test_fp() != true) return 1;

    // Clean the relic core.
    core_clean();

    return 0;
}