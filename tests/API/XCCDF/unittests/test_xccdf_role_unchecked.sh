#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

echo "Stderr file = $stderr"
echo "Result file = $result"
$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="notselected"]'
assert_exists 0 '//rule-result/message'

rm $result
