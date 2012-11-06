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

assert_exists() { [ "$($XPATH $result 'count('"$2"')')" == "$1" ]; }

assert_exists 2 '//fix'
assert_exists 1 '//Rule/fix'
assert_exists 1 '//rule-result/fix'
assert_exists 1 '//Rule/fix/@id'
assert_exists 1 '//Rule/fix/@*'
assert_exists 1 '//rule-result/fix/@id'
assert_exists 1 '//rule-result/fix/@*'

rm -rf $result
