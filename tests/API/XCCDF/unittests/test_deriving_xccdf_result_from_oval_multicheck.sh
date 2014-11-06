#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

assert_exists 8 '//rule-result'
assert_exists 8 '//rule-result/result'
assert_exists 8 '//rule-result/result[text()="pass"]'
assert_exists 8 '//rule-result/check'
assert_exists 8 '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
assert_exists 4 '//rule-result/check[@negate="true"]'
assert_exists 8 '//rule-result/check/check-content-ref'
assert_exists 4 '//rule-result/check/check-content-ref[@href="test_deriving_xccdf_result_from_oval_fail.oval.xml"]'
assert_exists 4 '//rule-result/check/check-content-ref[@href="test_deriving_xccdf_result_from_oval_pass.oval.xml"]'
assert_exists 0 '//message'

assert_exists 1 '//TestResult/score[@system="urn:xccdf:scoring:default"][text()="100.000000"]'
assert_exists 1 '//TestResult/score[@system="urn:xccdf:scoring:flat"][text()="2.000000"]'
assert_exists 1 '//TestResult[@version="1.0"]'

rm $result

