#!/bin/bash

set -e
set -o pipefail
set -x

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

$OSCAP xccdf validate-xml $result

assert_exists 1 '/Benchmark/Rule'
assert_exists 1 '/Benchmark/Rule/fix'
assert_exists 0 '/Benchmark/Rule/fix/sub'
assert_exists 1 '/Benchmark/Rule/fix/instance'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="fail"]'
assert_exists 0 '//rule-result/fix'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'
:> $result

ret=0
$OSCAP xccdf eval --remediate --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 2 ]
# filter out the expected warning in stderr
sed -i -E "/^W: oscap: The xccdf:rule-result\/xccdf:instance element was not found./d" "$stderr"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

assert_exists 1 '/Benchmark/Rule'
assert_exists 1 '/Benchmark/Rule/fix'
assert_exists 0 '/Benchmark/Rule/fix/sub'
assert_exists 1 '/Benchmark/Rule/fix/instance'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="fail"]'
assert_exists 1 '//rule-result/fix'
assert_exists 1 '//rule-result/fix[@system="urn:xccdf:fix:script:sh"]'
assert_exists 1 '//rule-result/fix/instance'
assert_exists 1 '//rule-result/message'
assert_exists 1 '//rule-result/message[@severity="info"]'
assert_exists 1 '//rule-result/message[text()="Fix execution was aborted: Text substitution failed."]'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'

[ ! -f test_file ]
rm $result
