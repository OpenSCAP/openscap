#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
output=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_xml_escaping_value.xccdf.xml 2>&1 > $output

echo "Output file = $output"
[ -s $output ] && rm -rf $output

echo "Result file = $result"
grep '<value selector="dod_short">I have read &amp; consent to terms in IS user agreement.</value>' $result \
	&& rm -rf $result
