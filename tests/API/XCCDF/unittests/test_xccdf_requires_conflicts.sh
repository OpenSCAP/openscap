#!/usr/bin/env bash

# Copyright 2021 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# Authors:
#      Evgenii Kolesnikov <ekolesni@redhat.com>

. "$builddir/tests/test_common.sh"

set -e -o pipefail

source="$srcdir/test_xccdf_requires_conflicts.xml"
result=result.xml

$OSCAP xccdf eval --profile xccdf_org.open-scap_profile_override --results "$result" "$source"

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="pass"]'

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1_2"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1_3"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_4"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_6"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_8"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_10"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_12"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_14"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_17"]/result[text()="notselected"]'

rm -f "$result"
