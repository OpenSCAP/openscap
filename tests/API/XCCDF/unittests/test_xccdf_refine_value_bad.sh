#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --profile PASS_MIN_LEN --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]
grep -q "^OpenSCAP Error: Invalid selector '20' for xccdf:value/@id='var-passwd_min_len'. Using null value instead." $stderr
rm $stderr

$OSCAP xccdf validate-xml $result

rm $result
