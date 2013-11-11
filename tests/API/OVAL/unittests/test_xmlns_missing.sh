#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

echo "Evaluating content."
res=0
$OSCAP info $srcdir/${name}.oval.xml 2> $stderr || res=$?
[ $res -eq 1 ]

grep 'Namespace prefix lin-def on object is not defined' $stderr
grep 'Could not find namespace definition for prefix' $stderr
grep 'Unknown test type oval:com.redhat.rhsa:tst:20100449002.' $stderr

rm $stderr
