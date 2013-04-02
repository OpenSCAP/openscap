#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
tmpdir=$(mktemp -d -t ${name}.out.XXXXXX)
result=$(mktemp -p $tmpdir ${name}.out.XXXXXX)
echo "Stderr file = $stderr"
echo "Result file = $stderr"
rm -f test_file

#
# First, ensure that fix is not applied if it is not applicable
#
ret=0
$OSCAP xccdf remediate --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]
$OSCAP xccdf validate $result
assert_exists 2 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fail"]'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'

#
# Second, make sure that the fix is applied, when CPE is recognized as appplicable
#
:> $result
$OSCAP xccdf remediate --cpe $srcdir/cpe-dict.xml --results $result $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ -f test_file ]; rm test_file
$OSCAP xccdf validate $result
assert_exists 2 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fixed"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@platform]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@platform="cpe:/o:example:applicable:5"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'

# Assert that input data was not modified.
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/title'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/title[text()="OSCAP Scan Result"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target[text()="x.x.example.com"]'
assert_exists 2 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-address'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-address[text()="127.0.0.1"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-address[text()="::1"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-facts'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-facts/fact'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/result[text()="fail"]'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/fix'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/message'

rm $result
