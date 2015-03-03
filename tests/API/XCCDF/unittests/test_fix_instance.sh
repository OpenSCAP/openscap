#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

rm -f test_file

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?

$OSCAP xccdf validate-xml $result

assert_exists 1 '/Benchmark/Rule/fix/instance'
assert_exists 3 '//rule-result'
assert_exists 2 '//rule-result/instance'
assert_exists 1 '//rule-result/instance[text()="a"]'
assert_exists 1 '//rule-result/instance[text()="b"]'
assert_exists 3 '//rule-result/result[text()="fail"]'

rm $result

# Remediate

$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?

$OSCAP xccdf validate-xml $result

assert_exists 1 '/Benchmark/Rule/fix/instance'
assert_exists 4 '//rule-result'
assert_exists 2 '//rule-result/instance[text()="a"]'
assert_exists 2 '//rule-result/instance[text()="b"]'
assert_exists 2 '//rule-result/fix'
assert_exists 2 '//rule-result/result[text()="fail"]'
assert_exists 2 '//rule-result/result[text()="fixed"]'

rm $result

