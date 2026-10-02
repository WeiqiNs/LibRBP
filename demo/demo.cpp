#include <iostream>
#include <rbp/rbp.hpp>

template <class C>
bool inner_product_in_the_exponent(){
    using namespace rbp;
    const auto x = random_vector<C>(10);
    const auto y = random_vector<C>(10);

    std::vector<G1<C>> left;
    std::vector<G2<C>> right;
    for (std::size_t i = 0; i < x.size(); ++i) {
        left.push_back(G1<C>::mul_generator(x[i]));
        right.push_back(G2<C>::mul_generator(y[i]));
    }

    const bool ok = pair(left, right) == Gt<C>::generator().pow(inner(x, y));
    std::cout << C::name << (ok ? ": pairing successful" : ": pairing failed") << std::endl;
    return ok;
}

template <class... Curves>
bool on_every_curve(){
    return (inner_product_in_the_exponent<Curves>() & ...);
}

int main(){
    return on_every_curve<rbp::BLS12_381, rbp::SS1536, rbp::BN254>() ? 0 : 1;
}
