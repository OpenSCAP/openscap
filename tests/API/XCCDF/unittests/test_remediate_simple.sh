#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

# Check the input data.
result=$srcdir/${name}.xccdf.xml
assert_exists 3 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]'

stderr=$(mktemp -t ${name}.out.XXXXXX)
tmpdir=$(mktemp -d -t ${name}.out.XXXXXX)
oval=test_remediation_simple.oval.xml
cp $srcdir/$oval $tmpdir # Accomodate scanning by xccdf placed in the same temp dir.
chmod u+w $tmpdir/$oval
result=$(mktemp -p $tmpdir ${name}.out.XXXXXX)
echo "Stderr file = $stderr"
echo "Result file = $stderr"
rm -f test_file

$OSCAP xccdf remediate --results $result  $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]
$OSCAP xccdf validate $result
assert_exists 4 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/rule-result/result[text()="notchecked"]'


$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile002 --results $result $result 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]
$OSCAP xccdf validate $result
assert_exists 5 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/rule-result/result[text()="notchecked"]'


ret=0
$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile001 --results $result $result 2> $stderr || ret=$?
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ -f test_file ]; rm test_file
$OSCAP xccdf validate $result
assert_exists 6 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/result[text()="error"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/fix'
assert_exists 2 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/message[text()="Fix execution comleted and returned: 0"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/message[text()="Failed to verify applied fix: Checking engine returns: notchecked"]'


$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile --results $result $result 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f test_file ]; rm test_file
$OSCAP xccdf validate $result
assert_exists 7 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/result[text()="fixed"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/fix'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/message[text()="Fix execution comleted and returned: 0"]'

rm $result
rm $tmpdir/$oval
rmdir $tmpdir
