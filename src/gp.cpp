#include "gp.hpp"

G1::G1(){
    g1_null(value);
    g1_new(value);
    g1_set_infty(value);
}

G1::G1(const G1& other){
    g1_new(value);
    g1_copy(value, other.value);
}

G1::G1(G1&& other) noexcept{
    g1_copy(value, other.value);
    other.free();
}

G1::~G1(){
    free();
}

G1& G1::operator=(const G1& other){
    if (this != &other)
        g1_copy(value, other.value);
    return *this;
}

G1& G1::operator=(G1&& other) noexcept{
    if (this != &other){
        g1_copy(value, other.value);
        other.free();
    }
    return *this;
}

void G1::free() const{
    if (value != nullptr) g1_free(value);
}

G2::G2(){
    g2_null(value);
    g2_new(value);
    g2_set_infty(value);
}

G2::G2(const G2& other){
    g2_new(value);
    g2_copy(value, other.value);
}

G2::G2(G2&& other) noexcept{
    g2_copy(value, other.value);
    other.free();
}

G2::~G2(){
    free();
}

G2& G2::operator=(const G2& other){
    if (this != &other)
        g2_copy(value, other.value);
    return *this;
}

G2& G2::operator=(G2&& other) noexcept{
    if (this != &other){
        g2_copy(value, other.value);
        other.free();
    }
    return *this;
}

void G2::free() const{
    if (value != nullptr) g2_free(value);
}

Gt::Gt(){
    gt_null(value);
    gt_new(value);
    gt_set_unity(value);
}

Gt::Gt(const Gt& other){
    gt_new(value);
    gt_copy(value, other.value);
}

Gt::Gt(Gt&& other) noexcept{
    gt_copy(value, other.value);
    other.free();
}

Gt::~Gt(){
    free();
}

Gt& Gt::operator=(const Gt& other){
    if (this != &other)
        gt_copy(value, other.value);
    return *this;
}

Gt& Gt::operator=(Gt&& other) noexcept{
    if (this != &other){
        gt_copy(value, other.value);
        other.free();
    }
    return *this;
}

void Gt::free() const{
    if (value != nullptr) gt_free(value);
}
