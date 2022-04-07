#!/usr/bin/env bash
set -e
set -o pipefail
set -x

# Regression test for https://bugzilla.redhat.com/show_bug.cgi?id=2033246

. $builddir/tests/test_common.sh

result=$(mktemp)
stderr=$(mktemp)
cp "$srcdir/binary_file" /tmp/

$OSCAP oval eval --results "$result" "$srcdir/test_binary_file.oval.xml"

# previous versions of OpenSCAP produce result="error"
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="true"]'
assert_exists 0 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="error"]'
! grep "Function pcre_exec() failed to match a regular expression with return code -10 on string .*" $stderr

rm -f "$result"
rm -f "$stderr"
rm -f /tmp/binary_file
