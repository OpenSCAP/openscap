#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
tmpdir=$(mktemp -d -t ${name}.out.XXXXXX)
result=$(mktemp -p $tmpdir ${name}.out.XXXXXX)
echo "Stderr file = $stderr"
echo "Result file = $stderr"
rm -f test_file test_file_cpe_na
rm -f wrong_test_file

#
# First, try without CPE, it should select different fix.
#
ret=0
$OSCAP xccdf remediate --results $result $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]
[ ! -f wrong_test_file ]
[ -f test_file_cpe_na ]

$OSCAP xccdf validate $result

assert_exists 2 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="error"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 2 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@*'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@system'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@system="urn:xccdf:fix:script:sh"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@reboot'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@reboot="true"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/text()[contains(., "touch test_file_cpe_na")]'
assert_exists 2 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Fix execution comleted and returned: 0"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Failed to verify applied fix: Checking engine returns: fail"]'
rm test_file_cpe_na

#
# Second, try with CPE and assert that result is different
#
:> $result
$OSCAP xccdf remediate --cpe $srcdir/cpe-dict.xml --results $result $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ -f test_file ]; rm test_file
[ ! -f test_file_cpe_na ]
[ ! -f wrong_test_file ]
$OSCAP xccdf validate $result
assert_exists 2 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fixed"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 3 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@*'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@platform]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@platform="cpe:/o:example:applicable:5"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@system]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@system="urn:xccdf:fix:script:sh"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@disruption]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@disruption="low"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Fix execution comleted and returned: 0"]'

# Assert that input data was not modified.
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/title'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/title[text()="OSCAP Scan Result"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target[text()="x.x.example.com"]'
assert_exists 2 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-address'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-address[text()="127.0.0.1"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-address[text()="0:0:0:0:0:0:0:1"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-facts'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/target-facts/fact'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/result[text()="fail"]'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/fix'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]/rule-result/message'

rm $result
