#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(make_temp_file /tmp ${name}.out)
tmpdir=$(make_temp_dir /tmp ${name}.out)
result=$(make_temp_file ${tmpdir} ${name}.out)

cpe=$srcdir/${name}.cpe.xml

echo "Stderr file = $stderr"
echo "Result file = $result"

$OSCAP xccdf eval --cpe $cpe --results $result $srcdir/${name}.xccdf.xml 2> $stderr
[ "$?" == "0" ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 '//TestResult'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_gtrue_rtrue"]/result[text()="pass"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_gtrue_rfalse"]/result[text()="notapplicable"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_gtrue_rnone"]/result[text()="pass"]'

assert_exists 1 '//TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_gfalse_rtrue"]/result[text()="notapplicable"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_gfalse_rfalse"]/result[text()="notapplicable"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_gfalse_rnone"]/result[text()="notapplicable"]'

rm $result
