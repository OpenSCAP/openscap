#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result --skip-valid $srcdir/test_skip_valid.oval.xml

rm $result

