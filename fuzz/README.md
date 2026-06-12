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

The harnesses above fuzz **SCAP XML documents** (the config a scanner consumes).
The harnesses below instead fuzz the **data an OVAL probe parses off the scanned
host's filesystem** — see [Probe-content fuzzing](#probe-content-fuzzing-scanned-filesystem).
They are built only when `-DENABLE_PROBES=ON`.

| Binary | Entry point (probe parser) | Corpus dir |
|--------|----------------------------|------------|
| `xinetd_probe_fuzzer`       | `xiconf_parse()` — the xinetd config-file parser | `corpus_probe_xinetd/` |
| `routingtable_probe_fuzzer` | `process_line_ip4()` / `process_line_ip6()` — `/proc/net/route` line parsers | `corpus_probe_routingtable/` |
| `shadow_probe_fuzzer`       | `parse_enc_mth()` — `/etc/shadow` hash-method classifier | `corpus_probe_shadow/` |
| `textfilecontent54_probe_fuzzer` | `process_file()` — file read + PCRE match loop (whole-file) | `corpus_probe_textfilecontent54/` |
| `textfilecontent_probe_fuzzer` | legacy `process_file()` — file read + PCRE match loop (per-line) | `corpus_probe_textfilecontent/` |
| `inetlisteningservers_probe_fuzzer` | `read_tcp()`/`read_udp()`/`read_raw()` — `/proc/net/{tcp,udp,raw}` parsers | `corpus_probe_inetlisteningservers/` |
| `iflisteners_probe_fuzzer` | `read_packet()` — `/proc/net/packet` parser | `corpus_probe_iflisteners/` |

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

## Probe-content fuzzing (scanned filesystem)

The XML harnesses fuzz the SCAP config, which on a real deployment is
static/trusted. The genuinely attacker-influenced input on a *scanned* host is
the data the OVAL probes read off that host's filesystem (`/etc/xinetd.d/*`,
`/proc/net/route`, `/etc/shadow`, text files, …). The `*_probe_fuzzer` harnesses
feed arbitrary bytes to those probe parsers.

These need the probe code compiled in, so use a **separate build** with probes
enabled (keep the XML-config `build/` as-is):

```sh
mkdir -p build-probe && cd build-probe
CC=clang CXX=clang++ cmake .. -DENABLE_FUZZING=ON -DENABLE_PROBES=ON -DENABLE_SCE=OFF
cmake --build . --target fuzzers -j"$(nproc)"
```

How they reach the parsers: each harness `#include`s the probe's `.c` to call
its `static` parser directly (the same trick `tests/probes/xinetd/` uses). The
probe's SEAP entry point (`*_probe_main`) is excluded or renamed so it does not
clash with the library or drag in the probe runtime, and `--gc-sections` drops
it. Bytes are fed via a tmpfs temp file (path parsers) or an in-memory buffer
(line/string parsers); see `probe_fuzz_common.h`.

Leak detection is meaningful here (the harnesses free per iteration), so run
these with `ASAN_OPTIONS=detect_leaks=1`:

```sh
cd build-probe
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=0 \
    ./fuzz/xinetd_probe_fuzzer -max_len=65536 ../fuzz/corpus_probe_xinetd
```

Two linking styles are used (both in `fuzz/CMakeLists.txt`):

- `add_fuzzer_probe()` — for parsers reachable with only exported symbols plus a
  couple of self-contained helper sources. It links the shared library and uses
  `--gc-sections` to drop the probe's `*_probe_main` (and the static helpers only
  it reaches), which would otherwise pull in non-exported probe-runtime symbols.
  Used by xinetd, routingtable, shadow.
- `add_fuzzer_probe_full()` — for parsers that themselves call non-exported
  helpers (`textfilecontent54`'s `process_file` → `probe_entobj_cmp` → the OVAL
  comparison code + item cache; `inetlisteningservers`'s `read_tcp`). The library
  hides those symbols (`C_VISIBILITY_PRESET hidden`), so instead of the `.so` we
  link the library's **object files** (visibility only affects the dynamic symbol
  table, not static linking) and pull in just `openscap`'s external link
  interface (libxml2, pcre2, …) via its `LINK_LIBRARIES` property. Linking the
  `.so` *on top of* the objects would define every global twice (an ASan
  ODR violation), so it is deliberately not linked.

`passwd` is skipped on glibc: its custom `oscap_fgetpwent()` parser is
`#ifdef`'d out in favour of libc's `fgetpwent()`.

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
