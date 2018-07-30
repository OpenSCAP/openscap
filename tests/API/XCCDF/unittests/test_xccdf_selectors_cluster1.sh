#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
output=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 --results $result $srcdir/${name}.xccdf.xml 2>&1 > $output

echo "Output file = $output"
echo "Result file = $result"
[ -f $output ]; [ "`cat $output`" == "XCCDF Results are exported correctly." ]; rm $output

$OSCAP xccdf validate $result

assert_exists 1 '//Profile'
assert_exists 2 '//Profile/select'
assert_exists 1 '//Profile/select[@selected="true"]'
assert_exists 1 '//Profile/select[@selected="false"]'
assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="notselected"]'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="0.000000"]'
rm $result
