#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

stderr=$(mktemp -t ${name}.err.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)

ds="$srcdir/test_reference_ds.xml"
p1="xccdf_com.example.www_profile_P1"

# Test 1: --list-vars with --profile prints value IDs and resolved values
$OSCAP info --profile $p1 --list-vars $ds > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep -q "xccdf_com.example.www_value_V1	42" $stdout
grep -q "xccdf_com.example.www_value_V2	custom_val" $stdout
# Verify output contains exactly 2 lines
[ "$(wc -l < $stdout)" -eq 2 ]
:> $stdout

# Test 2: --list-vars without --profile produces an error
$OSCAP info --list-vars $ds > $stdout 2> $stderr && exit 1 || true
grep -q "\-\-list-vars option requires \-\-profile" $stderr
:> $stdout
:> $stderr

rm -f $stdout $stderr
