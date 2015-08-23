#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
resultArf=$(mktemp -t ${name}.arf.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result --results-arf $resultArf $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result
$OSCAP ds rds-validate $resultArf

rm $result $resultArf
