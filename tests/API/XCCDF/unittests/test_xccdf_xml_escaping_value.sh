#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stdout=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_xml_escaping_value.xccdf.xml > $stdout 2> $stderr
echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
[ -f $stderr ]; [ ! -s $stderr ]; rm -rf $stderr
[ "`cat $stdout`" == "XCCDF Results are exported correctly." ]; rm -rf $stdout


echo "Result file = $result"
grep '<value selector="dod_short">I have read &amp; consent to terms in IS user agreement.</value>' $result
rm -rf $result
