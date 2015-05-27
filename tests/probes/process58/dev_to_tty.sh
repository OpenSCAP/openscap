#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

echo "Eval:"
$OSCAP oval eval --results $result $srcdir/$name.oval.xml 2> $stderr
[ ! -s $stderr ]

rm $stderr

[ -s $result ]
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="true"]'

rm $result
