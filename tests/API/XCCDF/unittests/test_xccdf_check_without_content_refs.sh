#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_without_content_refs.xccdf.xml 2> $stderr || [ $? == 2 ]

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 '/Benchmark/status[not(@date)]'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result[@idref="def-20120006"]/result[text()="notchecked"]'
assert_exists 0 '//rule-result/check/check-content-ref'
assert_exists 0 '//check[@multi-check]'
assert_exists 1 '/Benchmark/TestResult/score[@system="urn:xccdf:scoring:default"][text()="0.000000"]'
rm $result
