#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
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
daytime="$(date +%Y-%m-%d)T$(date +%H:)" # Format like '2013-02-27T15:01:57'
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]
$OSCAP xccdf validate $result
assert_exists 4 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/@start-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/@end-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002001"]/rule-result/result[text()="notchecked"]'


$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile002 --results $result $result 2> $stderr
daytime="$(date +%Y-%m-%d)T$(date +%H:%M)" # Format like '2013-02-27T15:01:57'
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]
$OSCAP xccdf validate $result
assert_exists 5 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/@start-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/@end-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002002"]/rule-result/result[text()="notchecked"]'


ret=0
$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile001 --results $result $result 2> $stderr || ret=$?
daytime="$(date +%Y-%m-%d)T$(date +%H:%M)" # Format like '2013-02-27T15:01:57'
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ -f test_file ]; rm test_file
$OSCAP xccdf validate $result
assert_exists 6 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/@start-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/@end-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/result[text()="error"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/fix'
assert_exists 2 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/message[text()="Fix execution comleted and returned: 0"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001001"]/rule-result/message[text()="Failed to verify applied fix: Checking engine returns: notchecked"]'


$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile --results $result $result 2> $stderr
daytime="$(date +%Y-%m-%d)T$(date +%H:%M)" # Format like '2013-02-27T15:01:57'
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f test_file ]; rm test_file
$OSCAP xccdf validate $result
assert_exists 7 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/@start-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/@end-time[contains(.,"'$daytime'")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/result[text()="fixed"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/fix'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile003"]/rule-result/message[text()="Fix execution comleted and returned: 0"]'


# Assert that input data was not modified.
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"][@start-time="2013-02-27T14:54:43"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"][@end-time="2013-02-27T14:54:44"]'
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
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"][@start-time="2013-02-27T15:01:56"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"][@end-time="2013-02-27T15:01:56"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fail"]'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"][@start-time="2013-02-27T15:01:57"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"][@end-time="2013-02-27T15:01:57"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]/rule-result/result[text()="notchecked"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]/rule-result/message'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile002"]/rule-result/fix'

rm $result
rm $tmpdir/$oval
rmdir $tmpdir
