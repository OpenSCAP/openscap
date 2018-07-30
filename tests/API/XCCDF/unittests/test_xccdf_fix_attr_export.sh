#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 3 '//fix'
assert_exists 3 '//fix/@id'
assert_exists 3 '//Rule/fix'
assert_exists 0 '//rule-result/fix'
assert_exists 1 '//Rule/fix[@id="fix-empty"]'
assert_exists 0 '//rule-result/fix[@id="fix-empty"]'
assert_exists 1 '//Rule/fix[@id="fix-unknown"]'
assert_exists 0 '//rule-result/fix[@id="fix-unknown"]'
assert_exists 1 '//Rule/fix[@id="fix-restrict"]'
assert_exists 0 '//rule-result/fix[@id="fix-restrict"]'

assert_exists 1 '//fix[@id="fix-empty"]'
assert_exists 1 '//fix[@id="fix-empty"]/@*'

assert_exists 1 '//fix[@id="fix-unknown"]'
assert_exists 3 '//fix[@id="fix-unknown"]/@*'
assert_exists 1 '//fix[@id="fix-unknown" and @disruption="unknown"]'
assert_exists 1 '//fix[@id="fix-unknown" and @complexity="unknown"]'
assert_exists 0 '//fix[@id="fix-unknown" and @strategy="unknown"]'

assert_exists 1 '//fix[@id="fix-restrict"]'
assert_exists 4 '//fix[@id="fix-restrict"]/@*'
assert_exists 1 '//fix[@id="fix-restrict" and @disruption="medium"]'
assert_exists 1 '//fix[@id="fix-restrict" and @complexity="high"]'
assert_exists 1 '//fix[@id="fix-restrict" and @strategy="restrict"]'

rm $result
