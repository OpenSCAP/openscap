#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp)
stderr=$(mktemp)

echo "Result file = $result"
echo "Stderr file = $stderr"

$OSCAP xccdf generate fix --fix-type bootc --profile common "$srcdir/test_remediation_bootc.ds.xml" > "$result" 2> "$stderr"
[ -e $stderr ]

diff -u "$srcdir/test_remediation_bootc_expected_output.sh" "$result"

rm -rf "$stdout" "$stderr" "$result"
