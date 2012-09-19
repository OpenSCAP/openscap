#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -s $stderr ] && rm -rf $stderr

$OSCAP xccdf validate-xml $result

assert_exists_once() { [ $(xpath $result 'count('$1')') == "1" ]; }
assert_exists_once '//rule-result'
assert_exists_once '//rule-result/result[text()="pass"]'
assert_exists_once '//rule-result/check'
assert_exists_once '//rule-result/check[not(@selector)]'
assert_exists_once '//rule-result/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
assert_exists_once '//rule-result/check/check-content-ref[@name="oval:moc.elpmaxe.www:def:1"]'
rm -rf $result
