#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e
set -o pipefail

stdout="$(mktemp)"
stderr="$(mktemp)"

$OSCAP oval eval --id "oval:x:def:1" $srcdir/test_circular_extend_def.xml > "$stdout" 2> "$stderr"

grep -q "Definition oval:x:def:1: not evaluated" "$stdout"
grep -q "Circular dependency in OVAL definition 'oval:x:def:1'\." "$stderr"

rm -f "$stdout"
rm -f "$stderr"

stdout="$(mktemp)"
stderr="$(mktemp)"

$OSCAP oval eval --id "oval:x:def:2" $srcdir/test_circular_extend_def.xml > "$stdout" 2> "$stderr"

grep -q "Definition oval:x:def:2: not evaluated" "$stdout"
grep -q "Circular dependency in OVAL definition 'oval:x:def:2'\." "$stderr"

rm -f "$stdout"
rm -f "$stderr"

stdout="$(mktemp)"
stderr="$(mktemp)"

$OSCAP oval eval $srcdir/test_circular_extend_def.xml > "$stdout" 2> "$stderr"

grep -q "Definition oval:x:def:1: not evaluated" "$stdout"
grep -q "Definition oval:x:def:2: not evaluated" "$stdout"
grep -q "Circular dependency in OVAL definition .*" "$stderr"

rm -f "$stdout"
rm -f "$stderr"
