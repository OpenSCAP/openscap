#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

echo "Analysing syschar content."
$OSCAP oval analyse --results $result $srcdir/$name.oval.xml $srcdir/$name.syschar.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f $result ]

assert_exists 1 '/oval_results'
assert_exists 1 '/oval_results/generator'
assert_exists 1 '/oval_results/generator/oval:product_name'
assert_exists 1 '/oval_results/generator/oval:product_name[text()="cpe:/a:open-scap:oscap"]'
assert_exists 1 '/oval_results/generator/oval:schema_version'
assert_exists 1 '/oval_results/generator/oval:schema_version[text()="5.8"]'
assert_exists 1 '/oval_results/generator/oval:timestamp'
assert_exists 1 '/oval_results/directives'
assert_exists 1 '/oval_results/oval_definitions'
assert_exists 1 '/oval_results/results'
assert_exists 1 '/oval_results/results/system'
assert_exists 1 '/oval_results/results/system/definitions'
assert_exists 1 '/oval_results/results/system/definitions/definition'
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria/criterion'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria/criterion[@result="true"]'
assert_exists 1 '/oval_results/results/system/tests'
assert_exists 1 '/oval_results/results/system/tests/test'
assert_exists 1 '/oval_results/results/system/tests/test[@result="true"]'
assert_exists 2 '/oval_results/results/system/tests/test/*'
assert_exists 1 '/oval_results/results/system/tests/test/tested_item'
assert_exists 2 '/oval_results/results/system/tests/test/tested_item/@*'
assert_exists 1 '/oval_results/results/system/tests/test/tested_item/@item_id'
assert_exists 1 '/oval_results/results/system/tests/test/tested_item[@result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test/tested_variable'
assert_exists 1 '/oval_results/results/system/tests/test/tested_variable/@*'
assert_exists 1 '/oval_results/results/system/tests/test/tested_variable[@variable_id="oval:x:var:1"]'
assert_exists 1 '/oval_results/results/system/tests/test/tested_variable[text()="700:1a7e:70:face:b055::f1ee:f1ee"]'

rm $result
