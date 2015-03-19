#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/comment.xml

assert_exists 1 '/oval_results/results/system/tests'
assert_exists 1 '/oval_results/results/system/tests/test'

rm $result

