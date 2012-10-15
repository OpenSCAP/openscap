#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm -rf $stderr

$OSCAP xccdf validate-xml $result

assert_exists_once() { [ $($XPATH $result 'count('"$1"')') == "1" ]; }
assert_exists_twice() { [ $($XPATH $result 'count('"$1"')') == "2" ]; }
assert_exists_once '//rule-result'
assert_exists_once '//rule-result/result'
assert_exists_once '//rule-result/result[text()="notchecked"]'
assert_exists_once '//rule-result/message'
assert_exists_once '//rule-result/message[@severity="info"]'
assert_exists_once '//rule-result/message[text()="No candidate or applicable check found."]'
assert_exists_twice '//rule-result/*'
rm -rf $result
