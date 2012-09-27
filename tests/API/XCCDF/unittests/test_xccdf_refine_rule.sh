#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm -rf $stderr

$OSCAP xccdf validate-xml $result

[ "$(xpath $result 'count(//refine-rule[@weight])')" == "0" ]
assert_exists_once() { [ "$(xpath $result 'count('$1')')" == "1" ]; }
assert_exists_once '//refine-rule[not(@weight)]'
assert_exists_once '//rule-result'
assert_exists_once '//rule-result/result'
assert_exists_once '//rule-result/result[text()="notchecked"]'
assert_exists_once '//rule-result[not(check)]'

rm -rf $result
