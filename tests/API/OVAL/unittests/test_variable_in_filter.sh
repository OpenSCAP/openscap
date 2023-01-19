#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=`mktemp`
stdout=`mktemp`
stderr=`mktemp`
echo "secret_key" > /tmp/key_file

$OSCAP oval eval --results "$result" "$srcdir/test_variable_in_filter.xml" > "$stdout" 2> "$stderr"
grep "Failed to convert OVAL state to SEXP" "$stderr" && exit 1
assert_exists 1 '//oval_results/results/system/definitions/definition[@result="true"]'
assert_exists 0 '//oval_results/results/system/definitions/definition[@result!="true"]'

rm -f "$result" "$stdout" "$stderr" /tmp/key_file
