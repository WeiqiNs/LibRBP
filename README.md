# RELIC based Bilinear Pairing Library (LibRBP)

[![LibRBP CI](https://github.com/WeiqiNs/LibRBP/actions/workflows/ci.yml/badge.svg)](https://github.com/WeiqiNs/LibRBP/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/WeiqiNs/LibRBP/graph/badge.svg?token=U1HZR28Q1Y)](https://codecov.io/gh/WeiqiNs/LibRBP)

LibRBP is a C++20 library for prototyping pairing-based cryptographic schemes on top of
[RELIC](https://github.com/relic-toolkit/relic). Every curve is a type, so one program can use asymmetric and symmetric
curves side by side, and generic scheme code is written once as a template.

| Curve | Tag | Pairing | Notes |
| --- | --- | --- | --- |
| `bls12_381` | `rbp::BLS12_381` | type-3, embedding degree 12 | The common default |
| `ss1536` | `rbp::SS1536` | type-1 (symmetric), embedding degree 2 | `pair(G1, G1)` is available |
| `bn254` | `rbp::BN254` | type-3, embedding degree 12 | About 100-bit security; for comparisons with older work. This is RELIC's BN-254, not Ethereum's alt_bn128 |

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

## What it provides

- `Zp<C>`: integers mod the group order r, with `+ - * /`, `inverse()`, `pow()`, hashing and fixed-width encoding.
  Hash functions take bytes; `rbp::bytes_of("text")` converts a string. `Zp::hash` domains are at most 255 bytes.
- `G1<C>`, `G2<C>`: additive groups with `+ -`, scalar `*`, `mul_generator` (RELIC's precomputed table) on one scalar
  or a vector, hashing, `msm`, `sum`, and validated compressed or uncompressed encodings.
- `Gt<C>`: the multiplicative target group with `* /`, `pow` and validated encodings. `dlog(base, target, lo, hi)` uses
  baby-step giant-step; build a `DlogTable<C>(base, lo, hi)` once to reuse its table across many lookups with the same
  base and range.
- `pair(p, q)`, the multi-pairing `pair(ps, qs)`, and `pair(p, q)` on two G1 points when `C::symmetric`.
- `Vector<C>` and `Matrix<C>`: vector operations, matrix products, transpose, determinant and inverse (Gauss-Jordan with
  pivoting), plus `poly_from_roots`.
- Typed errors: `ShapeError`, `NotInvertible`, `DecodeError` and `RelicError`, all derived from `rbp::Error`.
- Printing: `std::format("{}", x)` and `operator<<` show scalars in decimal and group elements as the hex of their
  encoding, so test failures are readable.

RELIC initializes itself on first use; there is no setup or teardown call. `rbp::seed<C>(bytes)` makes the random
sequence reproducible for tests and benchmarks. LibRBP is single-threaded, because RELIC is built without
multithreading support, and it is meant for research prototypes: it makes no constant-time guarantees.

## Building

LibRBP builds on Linux with CMake, a C++20 compiler, GMP (`libgmp-dev`) and git. LibRBP fetches RELIC and builds it
once per curve; GoogleTest is used from the system when available and fetched otherwise.

```bash
cmake -B build -S .
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

| Option | Default | Effect |
| --- | --- | --- |
| `RBP_CURVES` | `bls12_381;ss1536;bn254` | Curves to build, from the registry in `cmake/RBPCurves.cmake` |
| `RBP_RELIC_GIT_TAG` | the commit pinned in `cmake/RBPRelic.cmake` | RELIC branch, tag or commit; configure prints the resolved commit |
| `FETCHCONTENT_SOURCE_DIR_RELIC` | unset | Build from a local RELIC checkout instead of fetching |
| `RBP_BUILD_TESTS` | on when top-level | Build the test suite |
| `RBP_ENABLE_COVERAGE` | off | Build with `--coverage` |

Each curve becomes its own shared library (`RBP::BLS12_381`, `RBP::SS1536`) with its RELIC linked in statically and
hidden, which is what lets several curves share one process. `RBP::RBP` links every built curve. Each library's soname
ends in a hash of its element sizes (`libRBP_bls12_381.so.0.<hash>`), so a program built against one
RELIC layout refuses to load a library rebuilt with another instead of corrupting memory.

## Using it from another project

After `cmake --install`:

```cmake
find_package(RBP REQUIRED COMPONENTS bls12_381 ss1536)
target_link_libraries(app PRIVATE RBP::RBP)
```

Or without installing:

```cmake
include(FetchContent)
FetchContent_Declare(LibRBP GIT_REPOSITORY https://github.com/WeiqiNs/LibRBP.git GIT_TAG main)
FetchContent_MakeAvailable(LibRBP)
target_link_libraries(app PRIVATE RBP::RBP)
```

Both carry the C++20 requirement to `app`. The [demo](demo) folder is a complete consumer.

## Adding a curve

Register it in `cmake/RBPCurves.cmake` with `rbp_register_curve(<name> <RELIC preset>)`; the name must be a lowercase
C identifier and its tag type is the name in uppercase. Then add `test/fixtures/<name>.hpp` with the curve's group
order, its compressed G1 and G2 sizes, and an encoding of a point outside each subgroup (`std::nullopt` when the
cofactor is 1). Configure fails with a named error when either is missing. Not every RELIC preset works: some x86-64
assembly backends define their low-level symbols without RELIC's `LABEL` prefix, so the curve configures but its
library fails to link with an undefined `<name>_bn_*_low` symbol.

## Docker

`docker build -t librbp:dev .` builds everything from source, runs the tests, installs the package and builds the demo;
`docker run --rm librbp:dev` runs the demo on every curve.
