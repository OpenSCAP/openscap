#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]
[ "WARNING: Skipping $srcdir/_non_existent_.oval.xml file which is referenced from XCCDF content" == "`cat $stderr`" ]
rm $stderr

$OSCAP xccdf validate-xml $result

assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="notchecked"]'
assert_exists 0 '//rule-result/check'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'
rm $result
