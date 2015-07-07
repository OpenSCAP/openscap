#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/cim_datetime.xml

assert_exists 1 '/oval_results/results/system/tests'
assert_exists 3 '/oval_results/results/system/tests/test'
assert_exists 3 '/oval_results/results/system/tests/test[@result="true"]'
assert_exists 3 '/oval_results/results/system/tests/test/tested_variable[text()="3600"]'
assert_exists 3 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:variable_item'
assert_exists 3 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:variable_item[@status="exists"]'
assert_exists 3 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:variable_item/ind-sys:value[text()="3600"]'

rm $result

