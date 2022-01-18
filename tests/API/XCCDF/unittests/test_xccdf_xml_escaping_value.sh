#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_xml_escaping_value.xccdf.xml 2> $stderr
echo "Stderr file = $stderr"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

echo "Result file = $result"
grep '<value selector="dod_short">I have read &amp; consent to terms in IS user agreement.</value>' $result
rm $result
