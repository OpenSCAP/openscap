#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
sds=$(mktemp -t ${name}.sds.XXXXXX)
xccdf=test_remediate_fix_processing.xccdf.xml
stderr=$(mktemp -t ${name}.out.XXXXXX)
resultx=$(mktemp -t ${name}.xccdf.XXXXXX)
arf=$(mktemp -t ${name}.arf.XXXXXX)
echo "sds file: $sds"
echo "stderr file: $stderr"
echo "results file: $result"
rm -f test_file test_file_cpe_na wrong_test_file

$OSCAP ds sds-compose $srcdir/$xccdf $sds 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
$OSCAP ds sds-validate $sds

# First, try to apply a wrong fix due to missing CPE.
ret=0
$OSCAP xccdf eval --remediate --results $resultx --results-arf $arf $sds 2> $stderr || ret=$?
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]
[ ! -f test_file ]
[ ! -f wrong_test_file ]
[ -f test_file_cpe_na ]; rm test_file_cpe_na

$OSCAP xccdf validate $resultx
$OSCAP ds rds-validate $arf

result=$resultx
assert_exists 2 '//TestResult'
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
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Fix execution completed and returned: 0"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Failed to verify applied fix: Checking engine returns: fail"]'
result=$arf
assert_exists 3 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="error"]'

:> $result
:> $arf
$OSCAP xccdf eval --cpe $srcdir/cpe-dict.xml --remediate --results $resultx --results-arf $arf $sds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f test_file ]; rm test_file
[ ! -f wrong_test_file ]
[ ! -f test_file_cpe_na ]

$OSCAP xccdf validate $resultx
$OSCAP ds rds-validate $arf

result=$resultx
assert_exists 2 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fixed"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 3 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@*'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@system'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@system="urn:xccdf:fix:script:sh"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@disruption'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@disruption="low"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/@platform'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[@platform="cpe:/o:example:applicable:5"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/text()[contains(., "touch test_file")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Fix execution completed and returned: 0"]'
result=$arf
assert_exists 3 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fixed"]'

rm $resultx $arf $sds
