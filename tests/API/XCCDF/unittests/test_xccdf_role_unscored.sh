#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
report=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result --report $report $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
echo "Report file = $report"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f $report ]; [ -s $report ]; rm $report

$OSCAP xccdf validate-xml $result

assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="informational"]'
assert_exists 1 '//rule-result/check'
assert_exists 1 '//rule-result/check/check-content-ref'
assert_exists 1 '//score[@system="urn:xccdf:scoring:default" and text()="0.000000"]'
assert_exists 1 '//score[@system="urn:xccdf:scoring:flat" and text()="0.000000"]'
assert_exists 1 '//score[@system="urn:xccdf:scoring:flat-unweighted" and text()="0.000000"]'
assert_exists 1 '//score[@system="urn:xccdf:scoring:absolute" and text()="1.000000"]'

rm $result
