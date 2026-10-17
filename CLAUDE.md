# LibRBP

Typed, thread-safe C++20 pairing library over RELIC. Each curve in `RBP_CURVES` (registry: `cmake/RBPCurves.cmake`)
is a tag `rbp::<NAME>` plus a shared library `libRBP_<name>` with its own hidden RELIC; schemes are templates over the
tag. LibPFE (`WeiqiNs/LibPFE`) pulls `main` with `FetchContent` (or an installed RBP) and links `RBP::RBP`; as a
subproject `PROJECT_IS_TOP_LEVEL` is false, so tests are off and the build type is not forced.

## Commands

CI (`.github/workflows/ci.yml`) installs `build-essential cmake git libgmp-dev libgtest-dev lcov`. Configure fetches
RELIC at `RBP_RELIC_GIT_TAG` and configures one RELIC per curve under `<build>/relic/<curve>`, so the first is slow.

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DRBP_ENABLE_COVERAGE=ON      # coverage job
cmake --build build --parallel
ctest --test-dir build --output-on-failure                                  # or -R <regex>, or build/test/test_<module>
lcov --capture --directory build --output-file all.info --filter brace,blank \
     --ignore-errors mismatch,inconsistent,unused,negative
lcov --extract all.info "$PWD/src/*" "$PWD/include/rbp/*" --output-file coverage.info --ignore-errors unused
lcov --summary coverage.info --fail-under-lines 100
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release                              # package job: build, ctest, then
sudo cmake --install build && sudo ldconfig
cmake -B demo/build -S demo && cmake --build demo/build --parallel && ./demo/build/demo
cmake -B build -S . -DCMAKE_BUILD_TYPE=RelWithDebInfo -DRBP_ENABLE_TSAN=ON  # thread-sanitizer job: build, then
TSAN_OPTIONS=halt_on_error=1 ctest --test-dir build --output-on-failure
```

Locally use one build directory per configuration; without root, install with `--prefix <dir>` and configure the demo
with `-DCMAKE_PREFIX_PATH=<dir>`. CI lowers `vm.mmap_rnd_bits` to 28 before TSan, and its Codecov upload needs GitHub
OIDC. lcov must match the gcov of the compiler that built `build/`.

## Layout

- `include/rbp/core.hpp`: `Zp<C>`, `Point<C, Side>` (`G1`, `G2`), `Gt<C>`, `PreparedG2<C>`, `PairingProduct<C>`, `pair`,
  `msm`, `seed`, declared only. `errors.hpp`: `Error` subclasses. `linalg.hpp` (`Vector`, `Matrix`), `dlog.hpp`
  (`DlogTable`), `io.hpp` (`to_hex`, `operator<<`, `std::formatter`): header-only templates over that API.
- `src/relic.hpp`: the only RELIC-facing header; checks RELIC's build options and sizes against `detail::Tag` and
  defines `detail::Raw`, `detail::Runtime<C>`, `detail::batch_size`. `src/*.cpp`: one file per area; `point.cpp`'s
  `Ops<Side>` maps to RELIC's `g1_`/`g2_` calls; `pairing_symmetric.cpp` (`pair(G1, G1)`) builds only for symmetric
  curves; `prepared.cpp` holds `PreparedG2` and `PairingProduct`.
- `cmake/RBPCurves.cmake`: `rbp_register_curve`, `rbp_measure_curve`, `rbp_add_curve`. `cmake/RBPRelic.cmake`: fetch,
  patch, per-curve RELIC configure. `cmake/RBPInstall.cmake`: exports `RBP::RBP` and `RBP::<NAME>`, curves as
  `find_package` components. `cmake/*.hpp.in` generate `<build>/generated/`: the public tag `include/rbp/<curve>.hpp`,
  the private `detail::Tag` header, and the umbrella `include/rbp/rbp.hpp`.
- `test/CMakeLists.txt`: `TEST_SOURCES`, the generated `curves.hpp` type lists (`Curves`, `Points`, `SymmetricCurves`,
  `CurveTuple`), and the `check_exports.sh`, `check_tls.sh` and soname tests. `test/fixtures/<curve>.hpp`: known
  answers. `demo/`: consumer of the installed package.

## How it fits together

- `rbp_measure_curve` measures RELIC's element sizes into the tag; `Zp`, `Point`, `Gt` hold the element inline in
  `storage_[C::*_size]`, reached via `detail::raw`. Every constructor calls `detail::Runtime<C>::require()`.
- Each `src/*.cpp` compiles once per curve and ends in explicit instantiations over `detail::Tag`. A new member of an
  instantiated class needs nothing more; a new free function in `core.hpp` needs its own instantiation line, or
  consumers fail to link.
- New curve: `rbp_register_curve(<name> <RELIC preset> <BN_PRECI>)` plus `test/fixtures/<name>.hpp` (configure fails
  without it; fields in `test/fixtures/fixture.hpp`); `RBP_CURVES`' default, `demo/` and `README.md` name curves too.
  `BN_PRECI` sizes every `Zp`; existing entries use the preset's `BN_PRECI`, else `FP_PRIME` rounded up to 64 bits. Too
  small a value still builds and corrupts memory at run time.
- New test file: add it to `TEST_SOURCES` and `TYPED_TEST_SUITE` over a type list from `<curves.hpp>`.

## Design facts

- Curves coexist: each RELIC is built with `LABEL=<curve>` and linked with `--exclude-libs,ALL` into a hidden-visibility
  library. Pinned by ctest `exported_symbols` and `MultiCurveTest.CurvesKeepIndependentState`.
- The SONAME version is `<major>.<layout>`, a hash of the measured sizes and symmetry, so a library rebuilt with another
  RELIC layout will not load under stale headers. Pinned by ctest `soname_carries_layout_<curve>`.
- The fetched RELIC is never edited: `rbp_build_relic` builds from a symlink view whose `src/relic_core.c` makes
  `first_ctx` an ordinary global (configure fails if that line moves), so threads that never use a curve reserve no
  context; ctest `thread_local_storage` bounds static TLS. RELIC is reconfigured only when its fingerprint changes.
- `detail::Runtime<C>` is the only `core_set` caller: the first thread keeps `first_ctx`; others get a heap context via
  `core_set_thread_initializer`, owned by a pthread key and freed after `thread_local` destructors. `seed<C>` reseeds
  only the calling thread. Pinned by `test_runtime.cpp`, `test_thread_keys.cpp`, `test_concurrency.cpp`.
- RELIC runs with `CHECK` off as its `longjmp` would skip destructors; decoders poll `relic_failed()`. Decoding checks
  length, `Zp` range and subgroup membership (`DecodeError`); shape mismatches throw `ShapeError`. Pinned by
  `DecodingRejectsInvalidEncodings` and `EncodingIsFixedWidthBigEndianAndValidated`.
- `Zp::hash` is `md_xmd` with the domain as DST; `Point::hash` adds a side byte so `G1` and `G2` hash apart even on
  symmetric curves. The fixtures' known answers pin both encodings.
- On embedding-degree-12 curves `src/prepared.cpp` replays RELIC's optimal-ate Miller loop: `MillerSchedule::walk`
  drives both `PreparedG2` line storage and `PairingProduct` evaluation, one loop and final exponentiation in total.
  Other curves use `pc_map_sim` in `detail::batch_size` chunks. Pinned by `PairingTest`; its batch cases and
  `MsmMatchesTheNaiveSumAcrossBatches` size inputs one past `batch_size`, so resize them if it changes.
- `PairingProduct` keeps a pointer to each `PreparedG2` until `evaluate()`, so the rvalue `add` is deleted
  (`PairingProductRejectsTemporaryPreparedKeys`). `DlogTable` keys baby steps by `Gt::fingerprint` (pinned by `GtTest`).

## CI

On push and pull request to `main`: **coverage** (Debug ctest, 100% line coverage of `src/`, `include/rbp/`; Codecov),
**package** (Release ctest, install, demo against the installed package) and **thread-sanitizer** (LibRBP and RELIC
instrumented, verified with `nm` for `__tsan_func_entry`, then the whole suite with `halt_on_error=1`).
