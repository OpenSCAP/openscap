#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0

input_xml="$srcdir/${name}.xccdf.xml"

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf generate fix --fix-type bash --profile 'common' --output "$result" "$input_xml" 2> $stderr

grep -q "^\s*#.*Profile title on one line" "$result"
grep -q "^\s*#\s*Profile description" "$result"
grep -q "^\s*#\s*that spans two lines" "$result"

rm "$result"

$OSCAP xccdf generate fix --fix-type ansible --profile 'second' --output "$result" "$input_xml" 2> $stderr

grep -q "^\s*#.*Second profile title on one line" "$result"
grep -q "^\s*#\s*Profile description" "$result"
grep -q "^\s*#\s*that spans two lines" "$result"

rm "$result"
