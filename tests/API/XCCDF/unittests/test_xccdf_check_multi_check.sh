#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_multi_check.xccdf.xml 2> $stderr || [ $? == 2 ]

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -s $stderr ] && rm -rf $stderr

$OSCAP xccdf validate-xml $result

[ $(xpath $result 'count(//check[not(@multi-check)])') == "0" ]

assert_exists_once() { [ $(xpath $result 'count('$1')') == "1" ]; }
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_1"]/check[@multi-check="true"]'
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_2"]/check[@multi-check="true"]'
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_3"]/check[@multi-check="false"]'
assert_exists_once '//Rule[@id="xccdf_moc.elpmaxe.www_rule_4"]/check[@multi-check="false"]' && \
	rm -rf $result
