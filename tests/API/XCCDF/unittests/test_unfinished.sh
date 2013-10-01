#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.err.XXXXXX)
echo "Stderr file = $stderr"

$OSCAP info ${srcdir}/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]
[ -s $stderr ]
cat $stderr | head -n 1 | grep '^OpenSCAP Error:'

rm $stderr
