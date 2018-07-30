#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml > $stdout 2> $stderr || [ $? == 2 ]

echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep '^Result.*pass$' $stdout
grep '^Result.*fail$' $stdout
rm $stdout

$OSCAP xccdf validate $result

assert_exists 1 '//Rule[@id="xccdf_moc.elpmaxe.www_rule_1"]/check[@multi-check="true"]'
assert_exists 1 '//check-content-ref[not(@name)]'
assert_exists 2 '//rule-result'
assert_exists 2 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]'
assert_exists 1 '//rule-result[result/text()="fail"]'
assert_exists 1 '//rule-result[result/text()="pass"]'
assert_exists 2 '//rule-result/check'
assert_exists 2 '//rule-result/check[@multi-check="true"]'
assert_exists 2 '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
assert_exists 2 '//rule-result/check/check-content-ref'
assert_exists 2 '//rule-result/check/check-content-ref[@href="test_xccdf_check_content_ref_without_name_attr.oval.xml"]'
assert_exists 1 '//rule-result/check/check-content-ref[@name="oval:moc.elpmaxe.www:def:2"]'
assert_exists 1 '//rule-result[result/text()="fail"]/check/check-content-ref[@name="oval:moc.elpmaxe.www:def:2"]'
assert_exists 1 '//rule-result/check/check-content-ref[@name="oval:moc.elpmaxe.www:def:1"]'
assert_exists 1 '//rule-result[result/text()="pass"]/check/check-content-ref[@name="oval:moc.elpmaxe.www:def:1"]'
rm $result
