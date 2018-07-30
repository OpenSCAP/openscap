#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

[ "`$OSCAP xccdf eval $srcdir/${name}.xccdf.xml 2>&1`" == "" ]

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_3 --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 '//Benchmark'
assert_exists 1 '//Benchmark[@resolved="1"]'
assert_exists 3 '//Profile'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_1"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_1"]/select'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_1"]/select[@idref="my-enabled-cluster" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_2"]'
assert_exists 3 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_2"]/select'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_2"]/select[1][@idref="my-enabled-cluster" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_2"]/select[2][@idref="my-disabled-cluster" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_2"]/select[3][@idref="xccdf_moc.elpmaxe.www_rule_3" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_3"]'
assert_exists 3 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_3"]/select'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_3"]/select[1][@idref="my-disabled-cluster" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_3"]/select[2][@idref="xccdf_moc.elpmaxe.www_rule_3" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_moc.elpmaxe.www_profile_3"]/select[3][@idref="my-enabled-cluster" and @selected="true"]'
assert_exists 3 '//rule-result'
assert_exists 3 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="pass"]'
assert_exists 2 '//rule-result/result[text()="notselected"]'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="100.000000"]'
rm $result
