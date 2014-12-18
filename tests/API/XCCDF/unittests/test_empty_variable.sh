#!/bin/bash

set -e
set -o pipefail
set -x

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result --profile RHEL6-Default $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/check'
assert_exists 1 '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'

assert_exists 1 '//rule-result/check/check-content-ref'
assert_exists 1 '//rule-result/check/check-content-ref[@href="test_empty_variable.oval.xml"]'


rm $result
