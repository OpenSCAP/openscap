#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
tmpdir=$(mktemp -d -t ${name}.out.XXXXXX)
result=$(mktemp -p $tmpdir ${name}.out.XXXXXX)
cpe=$srcdir/${name}.cpe.xml
echo "Stderr file = $stderr"
echo "Result file = $result"

$OSCAP xccdf eval --cpe $cpe --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ "$ret" == "2" ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 '//TestResult'
assert_exists 1 '//TestResult/platform'
assert_exists 1 '//TestResult/platform[@idref="cpe:/o:xxx:yyy"]'


rm $result
