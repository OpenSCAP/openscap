#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]

$OSCAP xccdf validate-xml $result

assert_exists 8 '//rule-result'
assert_exists 8 '//rule-result/result'
assert_exists 8 '//rule-result/result[text()="pass"]'
assert_exists 8 '//rule-result/check'
assert_exists 8 '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
assert_exists 4 '//rule-result/check[@negate="true"]'
assert_exists 4 '//rule-result/check[not(@negate)]'
assert_exists 8 '//rule-result/check/check-content-ref'
assert_exists 4 '//rule-result/check/check-content-ref[@href="oval/pass/oval.xml"]'
assert_exists 4 '//rule-result/check/check-content-ref[@href="oval/fail/oval.xml"]'
assert_exists 4 '//rule-result/check[@negate="true"]/check-content-ref[@href="oval/fail/oval.xml"]'
assert_exists 0 '//message'
assert_exists 1 '//TestResult/score[@system="urn:xccdf:scoring:default"][text()="100.000000"]'
assert_exists 1 '//TestResult/score[@system="urn:xccdf:scoring:flat"][text()="8.000000"]'

#
# Now, create a datastream, evaluate, expect the same results and split DataStream correctly
#

sds=$(mktemp -t ${name}.ds.XXXXXX)
echo "Creating SDS file = $sds"

$OSCAP ds sds-compose $srcdir/${name}.xccdf.xml $sds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]

$OSCAP ds sds-validate $sds 2> $stderr
$OSCAP info $sds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]

$OSCAP info $sds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]

:>$result
$OSCAP xccdf eval --results $result $sds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]

assert_exists 8 '//rule-result'
assert_exists 8 '//rule-result/result'
assert_exists 8 '//rule-result/result[text()="pass"]'
assert_exists 8 '//rule-result/check'
assert_exists 8 '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
assert_exists 4 '//rule-result/check[@negate="true"]'
assert_exists 4 '//rule-result/check[not(@negate)]'
assert_exists 8 '//rule-result/check/check-content-ref'
assert_exists 4 '//rule-result/check/check-content-ref[@href="oval/pass/oval.xml"]'
assert_exists 4 '//rule-result/check/check-content-ref[@href="oval/fail/oval.xml"]'
assert_exists 4 '//rule-result/check[@negate="true"]/check-content-ref[@href="oval/fail/oval.xml"]'
assert_exists 0 '//message'
assert_exists 1 '//TestResult/score[@system="urn:xccdf:scoring:default"][text()="100.000000"]'
assert_exists 1 '//TestResult/score[@system="urn:xccdf:scoring:flat"][text()="8.000000"]'

rm $result

split=$(mktemp -d -t ${name}.sds.XXXXXX)
echo "Creating SDS split = $split"

$OSCAP ds sds-split $sds $split 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]
rm $sds

# The following rm commands ensure that the files are present
for dir in fail pass; do
	$OSCAP oval validate $split/oval/$dir/oval.xml 2> $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	rm $split/oval/$dir/oval.xml
	rmdir $split/oval/$dir
done
rmdir $split/oval

mangle="scap_org.open-scap_cref_"
$OSCAP xccdf validate $split/${mangle}${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
rm $split/${mangle}${name}.xccdf.xml
rmdir $split
