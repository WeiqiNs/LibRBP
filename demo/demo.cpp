#include <iostream>
#include <LibRBP/bp.hpp>

int main(){
    std::cout << "Testing import the RBP library..." << std::endl;

    const auto Bp = BP();

    const auto x = Bp.Zp->rand_vec(10);
    const auto y = Bp.Zp->rand_vec(10);

    const auto g1x = Bp.Gp->g1_raise(x);
    const auto g1y = Bp.Gp->g1_raise(y);

    const auto g2x = Bp.Gp->g2_raise(x);
    const auto g2y = Bp.Gp->g2_raise(y);

    if (Group::cmp_gt(Group::pair(g1x, g2y), Group::pair(g1y, g2x))){
        std::cout << "Pairing successful!" << std::endl;
    } else{
        std::cout << "Pairing failed!" << std::endl;
    }

    BP::close();
    return 0;
}
