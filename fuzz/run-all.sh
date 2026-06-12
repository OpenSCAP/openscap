#!/usr/bin/env bash
#
# Drive all OpenSCAP libFuzzer harnesses in parallel for an extended run.
#
# Each harness fuzzes its own corpus, writes crash/oom/timeout artifacts under
# findings/<harness>/, and logs to logs/<harness>.log. New interesting inputs
# are added back into the harness's corpus directory so progress carries over
# between runs.
#
# Usage:
#   fuzz/run-all.sh [duration_seconds]
#
# Environment overrides:
#   BUILD       build directory containing fuzz/<harness>   (default: ./build)
#   SCHEMAS     OSCAP_SCHEMA_PATH for validate_fuzzer        (default: ./schemas)
#   FORK        libFuzzer -fork child processes per harness  (default: 1)
#   MAXLEN      -max_len                                      (default: 65536)
#   RSS         -rss_limit_mb                                 (default: 4096)
#   UNITTMO     -timeout (per-input, seconds)                (default: 25)
#
# Runs in -fork mode so a crash/OOM/timeout in one input does not stop the run:
# libFuzzer recycles the child, records the artifact, and keeps fuzzing. This
# also bounds memory (children are restarted), which matters for validate_fuzzer
# whose libxml2/libxslt caches grow across inputs.
#
# Examples:
#   fuzz/run-all.sh 3600                 # one hour, one process each
#   JOBS=4 fuzz/run-all.sh 28800         # 8h, 4 workers per harness
#
set -u

# Resolve repo root from this script's location so it works from anywhere.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

DURATION="${1:-3600}"
BUILD="${BUILD:-${ROOT}/build}"
SCHEMAS="${SCHEMAS:-${ROOT}/schemas}"
FORK="${FORK:-1}"
MAXLEN="${MAXLEN:-65536}"
RSS="${RSS:-4096}"
UNITTMO="${UNITTMO:-25}"

FUZZ_DIR="${ROOT}/fuzz"
OUT="${FUZZ_DIR}/findings"
LOGS="${FUZZ_DIR}/logs"

# harness   corpus-dir
#
# The probe-content harnesses (*_probe_fuzzer) only exist when the build was
# configured with -DENABLE_PROBES=ON; they are skipped automatically below if
# the binary is absent (e.g. the XML-config build uses -DENABLE_PROBES=OFF).
HARNESSES=(
	"scap_parse_fuzzer:corpus"
	"xccdf_policy_fuzzer:corpus_xccdf"
	"validate_fuzzer:corpus"
	"arf_fuzzer:corpus_arf"
	"xccdf_tailoring_fuzzer:corpus_tailoring"
	"xinetd_probe_fuzzer:corpus_probe_xinetd"
	"routingtable_probe_fuzzer:corpus_probe_routingtable"
	"shadow_probe_fuzzer:corpus_probe_shadow"
	"textfilecontent54_probe_fuzzer:corpus_probe_textfilecontent54"
	"textfilecontent_probe_fuzzer:corpus_probe_textfilecontent"
	"inetlisteningservers_probe_fuzzer:corpus_probe_inetlisteningservers"
	"iflisteners_probe_fuzzer:corpus_probe_iflisteners"
)

# Sanitizer runtime options shared by every harness:
#  - detect_leaks=0: the parsers intentionally abandon allocations on rejected
#    input; leak reports would drown out memory-safety crashes.
#  - halt_on_error=0: the OVAL code has many benign function-pointer-cast UBSan
#    reports that would otherwise abort the run.
export ASAN_OPTIONS="detect_leaks=0:abort_on_error=1:${ASAN_OPTIONS:-}"
export UBSAN_OPTIONS="halt_on_error=0:print_stacktrace=1:${UBSAN_OPTIONS:-}"

PIDS=()
cleanup() { echo; echo "[run-all] stopping…"; kill "${PIDS[@]}" 2>/dev/null; }
trap cleanup INT TERM

echo "[run-all] duration=${DURATION}s fork/harness=${FORK} build=${BUILD}"
mkdir -p "${LOGS}"

for entry in "${HARNESSES[@]}"; do
	name="${entry%%:*}"
	corpus="${FUZZ_DIR}/${entry##*:}"
	bin="${BUILD}/fuzz/${name}"

	if [[ ! -x "${bin}" ]]; then
		echo "[run-all] SKIP ${name}: not built (run: cmake --build '${BUILD}' --target fuzzers)"
		continue
	fi
	mkdir -p "${OUT}/${name}" "${corpus}"

	# validate_fuzzer needs the bundled schemas to reach the deep schema code.
	schema_env=()
	[[ "${name}" == "validate_fuzzer" ]] && schema_env=(env "OSCAP_SCHEMA_PATH=${SCHEMAS}")

	# -fork mode writes its own fuzz-<n>.log files into the cwd, so give each
	# harness its own working directory under logs/.
	workdir="${LOGS}/${name}.work"
	mkdir -p "${workdir}"

	echo "[run-all] launch ${name}  (corpus: ${entry##*:})"
	(
		cd "${workdir}" || exit 1
		"${schema_env[@]}" "${bin}" \
			-fork="${FORK}" \
			-ignore_crashes=1 -ignore_ooms=1 -ignore_timeouts=1 \
			-max_total_time="${DURATION}" \
			-max_len="${MAXLEN}" \
			-rss_limit_mb="${RSS}" \
			-timeout="${UNITTMO}" \
			-print_final_stats=1 \
			-artifact_prefix="${OUT}/${name}/" \
			"${corpus}"
	) > "${LOGS}/${name}.log" 2>&1 &
	PIDS+=("$!")
done

if [[ ${#PIDS[@]} -eq 0 ]]; then
	echo "[run-all] nothing to run."
	exit 1
fi

echo "[run-all] ${#PIDS[@]} harness(es) running; logs in ${LOGS}/"
wait "${PIDS[@]}"
trap - INT TERM

echo
echo "[run-all] ===== summary ====="
total=0
for entry in "${HARNESSES[@]}"; do
	name="${entry%%:*}"
	# crash-/oom-/leak-/timeout- artifacts indicate findings; ignore corpus units.
	mapfile -t finds < <(find "${OUT}/${name}" -maxdepth 1 -type f \
		\( -name 'crash-*' -o -name 'oom-*' -o -name 'leak-*' -o -name 'timeout-*' \) 2>/dev/null)
	n=${#finds[@]}
	total=$((total + n))
	if [[ ${n} -gt 0 ]]; then
		echo "  ${name}: ${n} finding(s)"
		for f in "${finds[@]}"; do echo "      ${f}"; done
	else
		echo "  ${name}: clean"
	fi
done
echo "[run-all] total findings: ${total}"
echo "[run-all] reproduce with: <harness> <artifact-file>  (validate_fuzzer needs OSCAP_SCHEMA_PATH=${SCHEMAS})"
exit 0
