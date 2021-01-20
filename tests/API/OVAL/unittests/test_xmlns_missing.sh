#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

res=0
$OSCAP info $srcdir/${name}.oval.xml 2> $stderr || res=$?

grep 'Namespace prefix lin-def on object is not defined' $stderr
grep 'Could not find namespace definition for prefix' $stderr

rm $stderr
