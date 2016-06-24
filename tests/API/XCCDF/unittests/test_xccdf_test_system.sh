#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

openscap_cpe="cpe:/a:redhat:openscap:"

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

assert_exists 1 '//TestResult'
assert_exists 1 '//TestResult[@test-system]'
assert_exists 1 '//TestResult[starts-with(@test-system,"'$openscap_cpe'")]'

rm $result
