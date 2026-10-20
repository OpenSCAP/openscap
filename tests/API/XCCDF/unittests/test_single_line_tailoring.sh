#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=$(mktemp)

$OSCAP xccdf eval --profile "xccdf_com.example.www_profile_custom" --results "$result" --tailoring-file "$srcdir/test_single_line_tailoring.tailoring.xml" "$srcdir/test_single_line_tailoring.xml"

assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'

rm -f "$result"
