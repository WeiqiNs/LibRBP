#include "fp.hpp"

Fp::Fp(){
    bn_new(value);
    bn_null(value);
    bn_zero(value);
}

Fp::Fp(const int x){
    bn_new(value);
    bn_null(value);
    bn_set_dig(value, x);
}

Fp::Fp(const std::string& x){
    bn_new(value);
    bn_null(value);
    bn_read_str(value, x.data(), x.size(), 10);
}

Fp::Fp(const std::vector<unsigned char>& x){
    bn_new(value);
    bn_null(value);
    bn_read_bin(value, x.data(), x.size());
}

Fp::Fp(const Fp& other){
    bn_new(value);
    bn_copy(value, other.value);
}

Fp::Fp(Fp&& other) noexcept{
    bn_copy(value, other.value);
    other.free();
}

Fp::~Fp(){
    free();
}

Fp& Fp::operator=(const Fp& other){
    if (this != &other) bn_copy(value, other.value);
    return *this;
}

Fp& Fp::operator=(Fp&& other) noexcept{
    if (this != &other){
        bn_copy(value, other.value);
        other.free();
    }
    return *this;
}

void Fp::free() const{
    if (value != nullptr) bn_free(value);
}
