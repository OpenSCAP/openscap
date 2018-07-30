#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml > $stdout 2> $stderr

echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ "`cat $stderr`" == "WARNING: Skipping $srcdir/nonexistent-file file which is referenced from XCCDF content" ]; rm $stderr
grep '^Result.*notchecked$' $stdout
rm $stdout

$OSCAP xccdf validate $result

assert_exists 1 '//Rule[@id="xccdf_moc.elpmaxe.www_rule_1"]/check[@multi-check="true"]'
assert_exists 2 '//check-content-ref[not(@name)]'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]'
assert_exists 1 '//rule-result[result/text()="notchecked"]'
assert_exists 1 '//rule-result/check'
assert_exists 1 '//rule-result/check[@multi-check="true"]'
assert_exists 1 '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
assert_exists 1 '//rule-result/check/check-content-ref[@href="test_xccdf_check_content_ref_without_name_attr.oval.xml" and @name="nonexistent"]'
assert_exists 1 '//rule-result/check/check-content-ref[@href="nonexistent-file" and @name="x"]'
assert_exists 1 '//rule-result/check/check-content-ref[@href="oval_without_definitions.oval.xml" and @name="nonexistent"]'
assert_exists 1 '//rule-result/check/check-content-ref[@href="nonexistent-file" and not(@name)]'
assert_exists 1 '//rule-result/message/@*'
assert_exists 1 '//rule-result/message[@severity="info"]'
assert_exists 1 '//rule-result/message[text()="None of the check-content-ref elements was resolvable."]'
assert_exists 1 '/Benchmark/TestResult/score'
assert_exists 2 '/Benchmark/TestResult/score/@*'
assert_exists 1 '/Benchmark/TestResult/score[@maximum="100.000000"]'
assert_exists 1 '/Benchmark/TestResult/score[text()="0.000000"]'

rm $result
