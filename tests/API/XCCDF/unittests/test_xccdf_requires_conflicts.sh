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
stderr="$(mktemp)"

$OSCAP xccdf eval --profile xccdf_org.open-scap_profile_override --results "$result" "$source" 2> "$stderr"
[ ! -s "$stderr" ]

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


# test the "conflicts" element with the --rule option

# rule 2 should be evaluated despite it conflicts with rule 3 but the user
# requested the rule explicetely

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_2 --results "$result" "$source" 2> "$stderr"
[ ! -s "$stderr" ]

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
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_4"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_6"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_8"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_10"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_12"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_14"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_17"]/result[text()="notselected"]'


# add --profile to the previous test

# same as previous, rule 2 should be evaluated despite it conflicts with rule 3
# but the user requested the rule explicetely, selecting a specific profile
# doesn't change anything

$OSCAP xccdf eval --profile xccdf_org.open-scap_profile_override --rule xccdf_moc.elpmaxe.www_rule_2 --results "$result" "$source" 2> "$stderr"
[ ! -s "$stderr" ]

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
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_4"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_6"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_8"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_10"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_12"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_14"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_17"]/result[text()="notselected"]'

# test conflicts element and --rule option

# Normally rule 2 shouldn't be evaluated because rule 2 conflicts with rule 3,
# but rule 2 will be evaluated because it's requested by the --rule option. Rule
# 4 isn't evaluated despite it's required by rule 3 because it wasn't listed on
# the command line. Once you provide the --rule option, oscap evaluates all the
# rules explicitely listed.

$OSCAP xccdf eval  --rule xccdf_moc.elpmaxe.www_rule_2 --rule xccdf_moc.elpmaxe.www_rule_3 --results "$result" "$source"  2> "$stderr"
grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_3' requires rule 'xccdf_moc.elpmaxe.www_rule_4', but it hasn't been specified using the '--rule' option" "$stderr"

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
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_4"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_6"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_8"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_10"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_12"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_14"]/result[text()="notselected"]'
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_17"]/result[text()="notselected"]'

# add --profile to the previous test

# Normally rule 2 shouldn't be evaluated because rule 2 conflicts with rule 3,
# but rule 2 will be evaluated because it's requested by the --rule option. Rule
# 4 isn't evaluated despite it's required by rule 3 because it wasn't listed on
# the command line. Once you provide the --rule option, oscap evaluates all the
# rules explicitely listed. Selecting a specific profile doesn't influence the
# rule selection performed by --rule.

$OSCAP xccdf eval  --rule xccdf_moc.elpmaxe.www_rule_2 --rule xccdf_moc.elpmaxe.www_rule_3 --profile xccdf_org.open-scap_profile_override --results "$result" "$source" 2> "$stderr"
grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_3' requires rule 'xccdf_moc.elpmaxe.www_rule_4', but it hasn't been specified using the '--rule' option" "$stderr"

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
assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="pass"]'
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

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_3 --results "$result" "$source" 2> "$stderr"
grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_3' requires rule 'xccdf_moc.elpmaxe.www_rule_4', but it hasn't been specified using the '--rule' option." "$stderr"

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

$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_3 --rule xccdf_moc.elpmaxe.www_rule_4 --results "$result" "$source"  2> "$stderr"
! grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_3' requires rule 'xccdf_moc.elpmaxe.www_rule_4', but it hasn't been specified using the '--rule' option." "$stderr"
grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_4' requires rule 'xccdf_moc.elpmaxe.www_rule_1', but it hasn't been specified using the '--rule' option." "$stderr"

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
$OSCAP xccdf eval --rule xccdf_moc.elpmaxe.www_rule_3 --rule xccdf_moc.elpmaxe.www_rule_4 --profile xccdf_org.open-scap_profile_override --results "$result" "$source" 2> "$stderr"
! grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_3' requires rule 'xccdf_moc.elpmaxe.www_rule_4', but it hasn't been specified using the '--rule' option." "$stderr"
grep "W: oscap: Rule 'xccdf_moc.elpmaxe.www_rule_4' requires rule 'xccdf_moc.elpmaxe.www_rule_1', but it hasn't been specified using the '--rule' option." "$stderr"

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

rm -f "$result"
rm -f "$stderr"
