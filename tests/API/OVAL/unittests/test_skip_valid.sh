#!/usr/bin/env bash
. $builddir/tests/test_common.sh

result=`mktemp`
stderr=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result --skip-valid $srcdir/test_skip_valid.oval.xml 2> $stderr

grep -q "Unable to export unknown state" $stderr

rm $result
rm $stderr

