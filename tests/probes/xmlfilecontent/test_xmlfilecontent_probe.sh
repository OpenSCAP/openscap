#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e -o pipefail

cp $srcdir/example.xml /tmp/
result=$(mktemp)
$OSCAP oval eval --results $result $srcdir/test_xmlfilecontent_probe.xml
# Even if OSCAP_FULL_VALIDATION is set, an invalid OVAL result doesn't cause
# the "oscap oval eval" to return a non-zero value, so let's run validation
# as a separate command
$OSCAP oval validate "$result"
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:2" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:5" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:6" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:7" and @result="true"]'
rm -f $result