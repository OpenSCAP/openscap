#! /bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/test_glob_to_regex.xml || exit 1

rm $result

