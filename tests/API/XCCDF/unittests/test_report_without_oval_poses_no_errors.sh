#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.output.XXXXXX)
stderr=$(mktemp -t ${name}.stderr.XXXXXX)
stdout=$(mktemp -t ${name}.stdout.XXXXXX)

$OSCAP xccdf generate report --output $result $srcdir/${name}.xccdf.xml.result.xml 2> $stderr > $stdout

echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f $result ]; ! grep '<div class="oval-results">' $result; rm $result
