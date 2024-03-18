#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

touch not_executable

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 --results $result $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate --skip-schematron $result

assert_exists 1 '//Profile'
assert_exists 1 '//Profile/select'
assert_exists 1 '//Profile/select[@selected="true"]'
assert_exists 1 '//Group'
assert_exists 1 '//Group[@selected="false"]'
assert_exists 4 '//Rule'
assert_exists 3 '//Benchmark/Rule'
assert_exists 1 '//Group/Rule'
assert_exists 3 '//Benchmark/Rule[@selected="false"]'
assert_exists 1 '//Group/Rule[@selected="true"]'
assert_exists 4 '//rule-result'
assert_exists 4 '//rule-result/result'
assert_exists 4 '//rule-result/result[text()="pass"]'
assert_exists 1 '//score'
assert_exists 1 '//score[text()="100.000000"]'
rm $result

rm not_executable
