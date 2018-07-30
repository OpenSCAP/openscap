#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0

rm -f test_file
# First make sure that this OVAL fails, then scan again with --remediate
$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 2 ]

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 '//Value'
assert_exists 3 '//Value/value'
assert_exists 1 '//Value/value[not(@selector)]'
assert_exists 1 '/Benchmark/Rule'
assert_exists 1 '/Benchmark/Rule/fix'
assert_exists 2 '/Benchmark/Rule/fix/sub'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="fail"]'
assert_exists 0 '//rule-result/fix'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'
:> $result

$OSCAP xccdf eval --remediate --results $result $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 '//Value'
assert_exists 3 '//Value/value'
assert_exists 1 '//Value/value[not(@selector)]'
assert_exists 1 '/Benchmark/Rule'
assert_exists 1 '/Benchmark/Rule/fix'
assert_exists 2 '/Benchmark/Rule/fix/sub'
assert_exists 1 '//TestResult'
assert_exists 0 '//TestResult/profile'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="fixed"]'
assert_exists 1 '//rule-result/fix'
assert_exists 1 '//rule-result/fix[@system="urn:xccdf:fix:script:sh"]'
assert_exists 1 '//rule-result/fix[contains(text(), "touch test_file")]'
assert_exists 1 '//rule-result/fix[contains(text(), "chmod a-x test_file")]'
assert_exists 0 '//rule-result/fix/sub'
assert_exists 1 '//rule-result/message'
assert_exists 1 '//rule-result/message[@severity="info"]'
assert_exists 1 '//rule-result/message[text()="Fix execution completed and returned: 0"]'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="100.000000"]'

rm test_file
rm $result
