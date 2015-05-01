#! /bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/test_glob_to_regex.xml

assert_exists 2 '/oval_results/results/system/definitions/definition[@result="true"]'
rm $result

