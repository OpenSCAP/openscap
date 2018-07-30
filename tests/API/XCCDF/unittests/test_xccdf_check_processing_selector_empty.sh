#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 '//rule-result'
assert_exists 1 '//rule-result/result'
assert_exists 1 '//rule-result/result[text()="notchecked"]'
assert_exists 1 '//rule-result/message'
assert_exists 1 '//rule-result/message[@severity="info"]'
assert_exists 1 '//rule-result/message[text()="No candidate or applicable check found."]'
assert_exists 0 '//rule-result/check'
rm $result
