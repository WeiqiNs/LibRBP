# LibRBP for agents

C++20 pairing library over RELIC. Every curve is a type (`rbp::BLS12_381`, `rbp::SS1536`, `rbp::BN254`) and its own
shared library, and scheme code is a template instantiated per curve. The public API is `include/rbp/` plus the per-curve
headers generated from `cmake/curve.hpp.in` into the build tree; `README.md` is the user-facing overview.

## Commands

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DRBP_ENABLE_COVERAGE=ON   # the first configure builds RELIC per curve
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For ThreadSanitizer, configure a separate `build-tsan` with `-DCMAKE_BUILD_TYPE=RelWithDebInfo -DRBP_ENABLE_TSAN=ON`.
CI requires 100% line coverage of `src/` and `include/rbp/`, a clean ThreadSanitizer run, an install-and-demo build,
and the checks in `test/CMakeLists.txt` (exported symbols, static TLS budget, soname layout hash).

## Invariants

- The fetched RELIC source is never modified. Each curve builds from a symlink view whose `src/relic_core.c` makes the
  default context an ordinary global; `cmake/RBPRelic.cmake` owns that patch and fails configure if it no longer applies.
- `detail::Runtime<C>` in `src/runtime.cpp` owns thread contexts: one per thread on first use, freed after the thread's
  `thread_local` destructors. Nothing outside `src/runtime.cpp` calls `core_set`.
- Element sizes come from the measurements in `cmake/RBPCurves.cmake`, never literals; `src/relic.hpp` static-asserts
  them and the required RELIC build options.
- `src/prepared.cpp` replays RELIC's optimal-ate Miller loop; any change must keep `pair(ps, prepared) == pair(ps, qs)`
  on every curve (`test/test_pairing.cpp`).
- Decoding checks length, `Zp` range, and subgroup membership for points and `Gt`. Shape mismatches throw
  `rbp::ShapeError`.
- Coverage comes from tests that reach a branch, never from reshaping code. No comments in code.

## Adding a curve

Register it in `cmake/RBPCurves.cmake` with `rbp_register_curve(<name> <RELIC preset> <BN_PRECI>)` and add
`test/fixtures/<name>.hpp` (group order, compressed point sizes, an off-subgroup x or `std::nullopt`, known hashes of
`"message"` under domain `"domain"`). `BN_PRECI` sizes every `Zp`: use the preset's value, else the field size rounded
up to 64 bits for embedding degree at most 12, else RELIC's 2048. Too small a value corrupts memory during setup and
surfaces as a `RelicError`, not a build failure.
