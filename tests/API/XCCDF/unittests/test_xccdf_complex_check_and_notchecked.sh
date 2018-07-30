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

$OSCAP xccdf validate $result

assert_exists 2 '//complex-check'
assert_exists 2 '//complex-check[@operator="AND"]'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="notchecked"]'
assert_exists 1 '//rule-result/complex-check'
assert_exists 1 '//rule-result/complex-check[@operator="AND"]'
assert_exists 2 '//rule-result/complex-check/check'
assert_exists 2 '//rule-result/complex-check/check/check-content-ref'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'
rm $result
