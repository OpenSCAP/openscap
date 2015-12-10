#!/bin/bash

result=`mktemp`
stderr=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result --skip-valid $srcdir/test_skip_valid.oval.xml 2> $stderr

grep -q "\*\*INVALID\*\*_state" $result
grep -q "OpenSCAP Error: Invalid OVAL family" $stderr

rm $result
rm $stderr

