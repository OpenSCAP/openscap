#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?

echo "Stderr file = $stderr"
echo "Result file = $result"
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="error"]'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'
rm $result
