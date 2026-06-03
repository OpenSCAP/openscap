# OpenSCAP fuzzers

[libFuzzer](https://llvm.org/docs/LibFuzzer.html) harnesses that exercise the
SCAP file processing code paths (parse / resolve / validate). Requires a Clang
toolchain (libFuzzer ships with Clang).

## Available harnesses

| Binary | Entry point | Corpus dir |
|--------|-------------|------------|
| `scap_parse_fuzzer`     | `oscap_source_get_scap_type()` then the matching importer (DS, ARF, XCCDF, all OVAL kinds, CPE) | `corpus/` |
| `xccdf_policy_fuzzer`   | `xccdf_policy_model_new()` + `build_all_useful_policies()` + `xccdf_policy_resolve()` | `corpus_xccdf/` |
| `validate_fuzzer`       | `oscap_source_validate()` + `oscap_source_validate_schematron()` | `corpus/` |
| `arf_fuzzer`            | `ds_rds_session_*` — build the RDS index, walk reports/assets, extract reports | `corpus_arf/` |
| `xccdf_tailoring_fuzzer`| `xccdf_tailoring_import_source()` against an embedded benchmark | `corpus_tailoring/` |

Each harness is one `*_fuzzer.c` file in this directory. Corpora are seeded from
`tests/` and grown by the fuzzer; they are git-ignored (regenerable).

## Build

```sh
mkdir -p build && cd build
CC=clang CXX=clang++ cmake .. -DENABLE_FUZZING=ON -DENABLE_PROBES=OFF -DENABLE_SCE=OFF
cmake --build . --target fuzzers -j"$(nproc)"   # builds all harnesses
```

`ENABLE_FUZZING` instruments the whole library with
`-fsanitize=fuzzer-no-link,address,undefined` and links each harness with
`-fsanitize=fuzzer`. (`-DENABLE_PROBES=OFF -DENABLE_SCE=OFF` just trims the build.)

## Run the fuzz tests

Recommended sanitizer environment (LeakSanitizer is noisy on inputs the parser
intentionally rejects mid-parse; UBSan `halt_on_error=0` keeps benign
function-pointer-cast reports from aborting):

```sh
export ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=0
```

One harness on its corpus:

```sh
cd build
./fuzz/scap_parse_fuzzer  -max_len=65536 ../fuzz/corpus
./fuzz/xccdf_policy_fuzzer -max_len=65536 ../fuzz/corpus_xccdf
# validate_fuzzer needs the bundled schemas:
OSCAP_SCHEMA_PATH=$(pwd)/../schemas ./fuzz/validate_fuzzer -max_len=65536 ../fuzz/corpus
```

All harnesses in parallel (libFuzzer `-fork` mode; a crash/OOM/timeout in one
input is recorded and fuzzing continues). `run-all.sh` sets the sanitizer
options and `OSCAP_SCHEMA_PATH` automatically:

```sh
fuzz/run-all.sh 3600            # duration in seconds; one fork child per harness
FORK=4 fuzz/run-all.sh 28800    # 4 fork children per harness
```

Findings land in `fuzz/findings/<harness>/` (`crash-`/`oom-`/`timeout-`/`leak-`),
per-harness logs in `fuzz/logs/<harness>.log`; a per-harness summary is printed
at the end. Both dirs are git-ignored.

## Coverage

Build a second, coverage-instrumented tree, replay the corpus, and report with
`llvm-cov`:

```sh
mkdir -p build-cov && cd build-cov
CC=clang CXX=clang++ cmake .. -DENABLE_FUZZING=ON -DENABLE_PROBES=OFF -DENABLE_SCE=OFF \
    -DCMAKE_C_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
    -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
    -DCMAKE_EXE_LINKER_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
    -DCMAKE_SHARED_LINKER_FLAGS="-fprofile-instr-generate -fcoverage-mapping"
cmake --build . --target fuzzers -j"$(nproc)"

# Replay the corpus (-runs=0 just executes the inputs, no fuzzing):
LLVM_PROFILE_FILE=cov.profraw ASAN_OPTIONS=detect_leaks=0 \
    ./fuzz/scap_parse_fuzzer -runs=0 ../fuzz/corpus

llvm-profdata merge -sparse cov.profraw -o cov.profdata
# The library lives in a shared object, so pass it with -object:
llvm-cov report ./fuzz/scap_parse_fuzzer -object ./src/libopenscap.so* \
    -instr-profile=cov.profdata
# Per-file/line detail:
llvm-cov show   ./fuzz/scap_parse_fuzzer -object ./src/libopenscap.so* \
    -instr-profile=cov.profdata src/OVAL/oval_parser.c
```

Merge several `*.profraw` (one per harness, via different `LLVM_PROFILE_FILE`)
before `report` to get combined coverage, and pass each harness with its own
`-object` to `llvm-cov`.

## Replay / debug a crash

A crashing input is written as `crash-<sha1>` (or `oom-`/`timeout-`) in the
working dir, or under `fuzz/findings/<harness>/` when using `run-all.sh`.
Curated regression inputs are in `fuzz/reproducers/`.

Replay one input through the harness that produced it — the ASan report
(stack trace, fault address, allocation site) prints to stderr:

```sh
cd build
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=0 \
    ./fuzz/scap_parse_fuzzer ./crash-<sha1>
# validate_fuzzer also needs: OSCAP_SCHEMA_PATH=$(pwd)/../schemas
```

> **Note:** a small number of reproducers (`crash-oval-set-mixed-type-double-free`
> and `crash-sds-index-checklist-null-strcmp`) trigger a UBSan
> wrong-function-pointer error rather than an ASan SEGV. Use
> `UBSAN_OPTIONS=halt_on_error=1` instead of `halt_on_error=0` for those.

Under a debugger — make ASan/UBSan abort into the debugger on the faulting frame:

```sh
ASAN_OPTIONS=abort_on_error=1:detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 \
    gdb --args ./fuzz/scap_parse_fuzzer ./crash-<sha1>
(gdb) run
(gdb) bt            # backtrace at the crash
# (lldb works the same: lldb -- ./fuzz/<harness> ./crash-<sha1>; run; bt)
```

Useful extras:
- Symbolized ASan traces need `llvm-symbolizer` on `PATH` (set
  `ASAN_SYMBOLIZER_PATH=$(command -v llvm-symbolizer)` if needed).
- Minimize a crash to the smallest triggering input:
  `./fuzz/<harness> -minimize_crash=1 -exact_artifact_path=min ./crash-<sha1>`.
- Replay all regression inputs (run from `build/`):
  ```sh
  for f in ../fuzz/reproducers/*; do
      ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=0 \
          ./fuzz/scap_parse_fuzzer "$f" >/dev/null 2>&1 || echo "triggered: $f"
  done
  ```
