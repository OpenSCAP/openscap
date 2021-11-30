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


# test the "conflicts" lemeent eith the --rule option

# oscap won't evaluate anything because rule 2 conflicts with rule 3 which is
# also selected in the default profile.

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_2 --results "$result" "$source"

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
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


# add --profile to the previous test

# same as previous, the profile doesn't change the selections of the rule 2 or 3, so the result is the same

$OSCAP xccdf eval --profile xccdf_org.open-scap_profile_override --rule xccdf_moc.elpmaxe.www_rule_2 --results "$result" "$source"

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
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

# test conflicts element and --rule option

# Evaluates only rule 3, rule 2 isn't evaluated because rule 2 conflicts with
# rule 3, rule 4 isn't evaluated despite it's required by rule 3 because it
# wasn't listed on the command line, once you provide the --rule option, oscap
# evaluates only the rules explicitly listed.

$OSCAP xccdf eval  --rule xccdf_moc.elpmaxe.www_rule_2 --rule xccdf_moc.elpmaxe.www_rule_3 --results "$result" "$source"

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
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

# add --profile to the previous test
# test conflicts element and the --rule option together with the --profile
# option, evaluates only rule 3, rule 2 isn't evaluated because rule 2 conflicts
# with rule 3, rule 4 isn't evaluated despite it's required by rule 3 because it
# wasn't listed on the command line, once you provide a --rule option, oscap
# evaluates only the rules explicitly listed, the profile doesn't change the
# selections of the rule 2 or 3, so the result is the same

$OSCAP xccdf eval  --rule xccdf_moc.elpmaxe.www_rule_2 --rule xccdf_moc.elpmaxe.www_rule_3 --profile xccdf_org.open-scap_profile_override --results "$result" "$source"

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
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

# test "requires" element and the --rule option
# evaluates only rule 3, despite the fact that rule 3 requires rule 4, but the
# rule 4 got skipped because it isn't listed, once you provide a --rule option,
# oscap evaluates only the rules explictely listed

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_3 --results "$result" "$source"

assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
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

# test "requires" element and the --rule option
# Evaluates both rule 3 and 4, it doesn't evaluate rule 1, despite rule 4
# requires rule 1, but rule 1 wasn't listed on command line.

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_3 --rule xccdf_moc.elpmaxe.www_rule_4 --results "$result" "$source"
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1_2"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_1_3"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_4"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_6"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_8"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_10"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_12"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_14"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_17"]/result[text()="notselected"]'

# add --profile to the previous test
# test "requires" element and the --rule option

# Evaluates only rule 3, it doesn't evaluate rule 4, because rule 4 requires
# rule 1, but rule 1 is unselected by the override profile therefore rule 4
# can't be evalauted

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_3 --rule xccdf_moc.elpmaxe.www_rule_4 --profile xccdf_org.open-scap_profile_override --results "$result" "$source"
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_5"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_7"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_9"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_11"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_13"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_15"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_16"]/result[text()="notselected"]'
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
