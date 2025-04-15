#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

xccdf="$srcdir/test_utils_args_xccdf_profile.xml"
stderr=$(mktemp)
results=$(mktemp)
verblofile=$(mktemp)

$OSCAP info --verbose INFO --profiles "$xccdf" >/dev/null 2> "$stderr"
grep -q "I: oscap: Identified document type: Benchmark" "$stderr"
$OSCAP info --profiles --verbose INFO "$xccdf" >/dev/null 2> "$stderr"
grep -q "I: oscap: Identified document type: Benchmark" "$stderr"
$OSCAP xccdf eval --verbose INFO --profile default --results "$results" "$xccdf" >/dev/null 2> "$stderr"
grep -q "I: oscap: Identified document type: Benchmark" "$stderr"
$OSCAP xccdf eval --profile default --verbose INFO --results "$results" "$xccdf" >/dev/null 2> "$stderr"
grep -q "I: oscap: Identified document type: Benchmark" "$stderr"
$OSCAP xccdf eval --profile default --results "$results" --verbose INFO "$xccdf" >/dev/null 2> "$stderr"
grep -q "I: oscap: Identified document type: Benchmark" "$stderr"
$OSCAP xccdf eval --verbose INFO --verbose-log-file "$verblofile" --profile default --results "$results" "$xccdf" >/dev/null 2>/dev/null
grep -q "I: oscap: Identified document type: Benchmark" "$verblofile"
$OSCAP xccdf eval --profile default --verbose INFO --verbose-log-file "$verblofile" --results "$results" "$xccdf" >/dev/null 2>/dev/null
grep -q "I: oscap: Identified document type: Benchmark" "$verblofile"
$OSCAP xccdf eval --profile default --results "$results" --verbose INFO --verbose-log-file "$verblofile" "$xccdf" >/dev/null 2>/dev/null
grep -q "I: oscap: Identified document type: Benchmark" "$verblofile"
$OSCAP xccdf eval --verbose INFO --profile default --results "$results" --verbose-log-file "$verblofile" "$xccdf" >/dev/null 2>/dev/null
grep -q "I: oscap: Identified document type: Benchmark" "$verblofile"
$OSCAP xccdf eval --verbose-log-file "$verblofile" --profile default --verbose INFO  --results "$results" "$xccdf" >/dev/null 2>/dev/null
grep -q "I: oscap: Identified document type: Benchmark" "$verblofile"
$OSCAP xccdf eval  --verbose INFO --profile default --results "$results" --verbose-log-file "$verblofile" "$xccdf" >/dev/null 2>/dev/null
grep -q "I: oscap: Identified document type: Benchmark" "$verblofile"

rm -f "$results" "$stderr" "$verblofile"
