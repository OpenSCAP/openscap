#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

echo "Eval:"
$OSCAP oval eval --results $result $srcdir/$name.oval.xml 2> $stderr
[ -f $stderr ];

rm $stderr

[ -f $result ]
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@flag="complete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:process58_item'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:pid[text()="1"]'

rm $result
