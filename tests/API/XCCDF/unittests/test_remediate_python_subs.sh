#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
result=$(mktemp -t ${name}.out.XXXXXX)
echo "Stderr file = $stderr"
echo "Result file = $stderr"
rm -f test_file

$OSCAP xccdf remediate --skip-valid --results $result $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f test_file ]; rm test_file

assert_exists 1 '//Rule'
assert_exists 1 '//Rule/fix'
assert_exists 1 '//Rule/fix[@system="urn:xccdf:fix:script:python"]'
assert_exists 0 '//Rule/fix/sub'
assert_exists 3 '//Rule/fix/xhtml:object'
assert_exists 2 '//TestResult'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/result[text()="fixed"]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/sub'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/object'
assert_exists 0 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix/xhtml:object'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[contains(text(), "import os")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/fix[contains(text(), "touch('"'"'test_file'"'"')")]'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message'
assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile001"]/rule-result/message[text()="Fix execution completed and returned: 0"]'

rm $result
