#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`
stdout=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_multi_check.xccdf.xml > $stdout 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm -rf $stderr
[ "`cat $stdout`" == "XCCDF Results are exported correctly." ]; rm -rf $stdout

$OSCAP xccdf validate-xml $result

[ $($XPATH $result 'count(//check[not(@multi-check)])') == "0" ]

assert_exists_once() { [ $($XPATH $result 'count('$1')') == "1" ]; }
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_1"]/check[@multi-check="true"]'
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_2"]/check[@multi-check="true"]'
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_3"]/check[@multi-check="false"]'
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_4"]/check[@multi-check="false"]'
rm -rf $result
