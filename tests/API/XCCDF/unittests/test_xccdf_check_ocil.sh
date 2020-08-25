#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`
$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_ocil.xml 2> $stderr
[ ! -s "$stderr" ]
$OSCAP xccdf validate $result
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]/result[text()="notchecked"]'
rm $stderr
rm $result

stderr=`mktemp`
$OSCAP xccdf eval --verbose INFO $srcdir/test_xccdf_check_ocil.xml 2> $stderr
grep -q "This rule requires an OCIL check. OCIL checks are not supported by OpenSCAP." $stderr
rm $stderr
