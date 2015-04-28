#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/test_invalid_regex.xml

assert_exists 1 '/oval_results/results/system/definitions/definition[@result="error"]'
rm $result

