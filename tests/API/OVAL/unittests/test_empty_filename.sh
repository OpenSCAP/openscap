#! /bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/empty_filename.xml

rm $result

