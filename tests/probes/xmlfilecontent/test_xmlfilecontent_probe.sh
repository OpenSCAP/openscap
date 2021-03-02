#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e -o pipefail

cp $srcdir/example.xml /tmp/
result=$(mktemp)
$OSCAP oval eval --results $result $srcdir/test_xmlfilecontent_probe.xml
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:2" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4" and @result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:5" and @result="true"]'
rm -f $result