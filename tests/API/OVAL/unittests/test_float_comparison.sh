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
assert_exists 4 '/oval_results/generator/*'
assert_exists 1 '/oval_results/generator/oval:product_name'
assert_exists 1 '/oval_results/generator/oval:product_version'
assert_exists 1 '/oval_results/generator/oval:product_name[text()="cpe:/a:open-scap:oscap"]'
assert_exists 1 '/oval_results/generator/oval:schema_version'
assert_exists 1 '/oval_results/generator/oval:schema_version[text()="5.8"]'
assert_exists 1 '/oval_results/generator/oval:timestamp'
assert_exists 1 '/oval_results/directives'
assert_exists 1 '/oval_results/oval_definitions'
assert_exists 0 '/oval_results/oval_definitions/generator/oval:product_version'
assert_exists 1 '/oval_results/results'
assert_exists 1 '/oval_results/results/system'
assert_exists 1 '/oval_results/results/system/definitions'
assert_exists 6 '/oval_results/results/system/definitions/definition'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1"][@result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:2"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:2"][@result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3"][@result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4"][@result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:5"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:5"][@result="true"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:6"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:6"][@result="true"]'

rm $result
