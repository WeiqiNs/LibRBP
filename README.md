# RELIC based Bilinear Pairing Library (LibRBP)

[![LibRBP CI](https://github.com/WeiqiNs/LibRBP/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/WeiqiNs/LibRBP/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/WeiqiNs/LibRBP/graph/badge.svg?token=U1HZR28Q1Y)](https://codecov.io/gh/WeiqiNs/LibRBP)

A typed, thread-safe C++20 library for pairing-based cryptography on
[RELIC](https://github.com/relic-toolkit/relic): bilinear pairings, group and field arithmetic, hashing to curves,
linear algebra and discrete-log tables on BLS12-381, BN254 and the symmetric SS1536. Every curve is its own type, so
one program can mix curves and write each scheme once as a template. It is the pairing layer under
[LibPFE](https://github.com/WeiqiNs/LibPFE), a functional encryption library.

```cpp
#include <rbp/rbp.hpp>

template <class C>
bool bilinear(){
    using namespace rbp;
    const auto a = Zp<C>::random(), b = Zp<C>::random();
    return pair(G1<C>::mul_generator(a), G2<C>::mul_generator(b)) == Gt<C>::generator().pow(a * b);
}

int main(){ return bilinear<rbp::BLS12_381>() && bilinear<rbp::SS1536>() ? 0 : 1; }
```

The example checks the bilinearity identity e(aP, bQ) = e(P, Q)^ab on two curves. That identity, multiplying two
secrets "in the exponent" exactly once, is what pairing-based schemes such as BLS signatures, identity-based
encryption, functional encryption and zk-SNARKs are built on, and the groups `G1`, `G2`, `Gt` and the scalar field
`Zp` are the vocabulary those papers use.

## Highlights

- **Several curves in one process.** Each curve is a shared library with its own RELIC linked in and hidden, which a
  single RELIC build cannot offer.
- **Thread-safe on top of a single-threaded C library.** Each thread gets a RELIC context the first time it touches a
  curve, and threads that never do pay nothing. CI runs the suite under ThreadSanitizer with RELIC instrumented too.
- **Fast repeated pairings.** `PreparedG2` stores a fixed key's Miller-loop lines once, and `PairingProduct` evaluates
  any mix of plain and prepared pairs in one shared Miller loop with one final exponentiation.
- **Safe at the boundary.** Every decode checks length and subgroup membership, and errors are typed (`ShapeError`,
  `DecodeError`, `NotInvertible`, `RelicError`).
- **100% line coverage**, plus CI checks that no RELIC symbol leaks from a library and that a library rebuilt with a
  different RELIC layout refuses to load.

## Curves

| Curve | Tag | Pairing |
| --- | --- | --- |
| BLS12-381 | `rbp::BLS12_381` | type 3, the common default |
| SS1536 | `rbp::SS1536` | type 1 (symmetric), so `pair(G1, G1)` works |
| BN254 | `rbp::BN254` | type 3, about 100-bit security, for comparison with older work |

## What's inside

- `Zp<C>`, `G1<C>`, `G2<C>`, `Gt<C>` with natural operators, hashing to each group, and validated encodings.
- `pair`, multi-pairings, `PreparedG2` and `PairingProduct`.
- `msm`, `mul_generator` on vectors, and `DlogTable` for discrete logarithms over a small range.
- `Vector<C>` and `Matrix<C>` with products, transpose, determinant and inverse.
- `std::format` and `operator<<` support, so test failures print readable values.

LibRBP is for research prototypes and makes no constant-time guarantees.

## Build and test

Linux, CMake 3.25+, a C++20 compiler, GMP (`libgmp-dev`) and git. RELIC is fetched and built once per curve.

```bash
cmake -B build -S .
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

Then, from another CMake project:

```cmake
find_package(RBP REQUIRED COMPONENTS bls12_381 ss1536)
target_link_libraries(app PRIVATE RBP::RBP)
```

`RBP_CURVES` picks the curves to build, and `cmake/RBPCurves.cmake` registers new ones. `docker build -t librbp .`
builds and tests everything from scratch, and [demo](demo) is a complete consumer.
