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
assert_exists 1 '/oval_results/generator/oval:schema_version[text()="5.3"]'
assert_exists 1 '/oval_results/generator/oval:timestamp'
assert_exists 1 '/oval_results/directives'
assert_exists 1 '/oval_results/oval_definitions'
assert_exists 1 '/oval_results/results'
assert_exists 1 '/oval_results/results/system'
assert_exists 1 '/oval_results/results/system/definitions'
assert_exists 1 '/oval_results/results/system/definitions/definition'
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="false"]'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria/criterion'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria/criterion[@result="false"]'
assert_exists 1 '/oval_results/results/system/tests'
assert_exists 1 '/oval_results/results/system/tests/test'
assert_exists 1 '/oval_results/results/system/tests/test[@result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test/tested_item'
assert_exists 1 '/oval_results/results/system/tests/test/tested_item[@result="false"]'

rm $result
