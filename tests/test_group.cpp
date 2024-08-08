#include "group.hpp"

bool test_pairing(){
    Fp x, y;
    bn_set_dig(x.value, 11);
    bn_set_dig(y.value, 13);

    const auto gp = Group();
    const auto r1 = Group::pair(gp.g1_raise(x), gp.g2_raise(y));
    const auto r2 = Group::pair(gp.g1_raise(y), gp.g2_raise(x));

    return Group::cmp_gt(r1, r2);
}

bool test_pairing_without_pre(){
    Fp x, y;
    bn_set_dig(x.value, 11);
    bn_set_dig(y.value, 13);

    const auto gp = Group(false);
    const auto r1 = Group::pair(gp.g1_raise(x), gp.g2_raise(y));
    const auto r2 = Group::pair(gp.g1_raise(y), gp.g2_raise(x));

    return Group::cmp_gt(r1, r2);
}

bool test_pairing_vec(){
    FpVec x(3), y(3);
    bn_set_dig(x[0].value, 11);
    bn_set_dig(x[1].value, 12);
    bn_set_dig(x[2].value, 13);
    bn_set_dig(y[0].value, 14);
    bn_set_dig(y[1].value, 15);
    bn_set_dig(y[2].value, 16);

    const auto gp = Group(false);
    const auto r1 = Group::pair(gp.g1_raise(x), gp.g2_raise(y));
    const auto r2 = Group::pair(gp.g1_raise(y), gp.g2_raise(x));

    return Group::cmp_gt(r1, r2);
}

int main(){
    // Initialize the relic core.
    core_init();
    pc_param_set_any();

    // Perform tests.
    if (test_pairing() != true) return 1;
    if (test_pairing_without_pre() != true) return 1;
    if (test_pairing_vec() != true) return 1;

    // Clean the relic core.
    core_clean();

    return 0;
}
