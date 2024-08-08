#include "fp.hpp"

Fp::Fp(){
    bn_new(value);
    bn_null(value);
    bn_zero(value);
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
