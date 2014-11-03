#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

./${name} ${srcdir}/${name}.arf.xml

result="${name}.xml.out"
echo "Result file = $result"
[ -f "$result" ]

$OSCAP ds rds-validate $result

assert_exists 1 '//TestResult'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/result'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/result[text()="pass"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override[@date]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override[@authority]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override/old-result'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override/old-result[text()="fail"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override/new-result'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override/new-result[text()="pass"]'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override/remark'
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_org.ssgproject.content_rule_disable_prelink"]/override/remark[text()="I believe this should have passed"]'

rm $result
