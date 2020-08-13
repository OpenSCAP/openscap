#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(make_temp_file /tmp ${name}.out)
stderr=$(make_temp_file /tmp ${name}.out)

ret=0

input_xml="$srcdir/${name}.xccdf.xml"

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf generate fix --fix-type bash --profile 'common' --output "$result" "$input_xml" 2> $stderr

grep -q "^#.*Profile title on one line" "$result"
grep -q "^#.*Profile description" "$result"
grep -q "^#.*that spans two lines" "$result"

rm "$result"

$OSCAP xccdf generate fix --fix-type ansible --profile 'second' --output "$result" "$input_xml" 2> $stderr

grep -q "^#.*Second profile title on one line" "$result"
grep -q "^#.*Profile description" "$result"
grep -q "^#.*that spans two lines" "$result"

rm "$result"
