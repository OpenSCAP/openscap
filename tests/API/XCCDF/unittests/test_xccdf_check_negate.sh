#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_negate.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

[ $($XPATH $result 'count(//complex-check)') == "0" ]

assert_exists_once() { [ $($XPATH $result 'count('$1')') == "1" ]; }
assert_exists_once '//Rule/check[@negate="true"]'
assert_exists_once '//rule-result/check[@negate="true"]'
assert_exists_once '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]/result[text()="pass"]'
rm $result
