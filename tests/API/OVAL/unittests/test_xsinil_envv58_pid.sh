#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "Result file: $result"

echo "Evaluating content."
$OSCAP oval eval --results $result $srcdir/${name}.oval.xml
echo "Validating results."
$OSCAP oval validate --results --schematron $result
echo "Testing results values."
assert_exists 1 '/oval_results'
assert_exists 1 '/oval_results/oval_definitions'
assert_exists 1 '/oval_results/oval_definitions/objects'
assert_exists 1 '/oval_results/oval_definitions/objects/*'
assert_exists 1 '/oval_results/oval_definitions/objects/*/ind-def:pid'
assert_exists 0 '/oval_results/oval_definitions/objects/*/ind-def:pid/*'
assert_exists 2 '/oval_results/oval_definitions/objects/*/ind-def:pid/@*'
assert_exists 1 '/oval_results/oval_definitions/objects/*/ind-def:pid[@datatype="int"]'
assert_exists 1 '/oval_results/oval_definitions/objects/*/ind-def:pid[@xsi:nil="true"]'

rm $result
