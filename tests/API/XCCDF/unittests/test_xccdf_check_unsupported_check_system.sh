#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_unsupported_check_system.xml 2> $stderr

$OSCAP xccdf validate-xml $result

assert_exists_once() { [ $($XPATH $result 'count('$1')') == "1" ]; }
assert_exists_once '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]/result[text()="notchecked"]'
rm -rf $result
