#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

ds="$srcdir/test_skip_rule.ds.xml"
p1="xccdf_com.example.www_profile_P1"
r1="xccdf_com.example.www_rule_R1"
r2="xccdf_com.example.www_rule_R2"
r3="xccdf_com.example.www_rule_R3"
r4="xccdf_com.example.www_rule_R4"
rx="xccdf_com.example.www_rule_nonexistent"

# Tests that all rules from profile P1 (profile contains only 4 rules) are
# evaluated when '--skip-rule' option is not specified.
$OSCAP xccdf eval --results $result --profile $p1 $ds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"pass\"]"
:> $result

# Tests that rule R1 is skipped and other rules from profile P1 are
# evaluated when the '--skip-rule' option is specified.
$OSCAP xccdf eval --results $result --profile $p1 \
    --skip-rule $r1 $ds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"pass\"]"
:> $result

# Tests that rule R1 and R2 are skipped and remaining rules (R3 and R4) from
# profile P1 are evaluated when the '--skip-rule $r1 --skip-rule $r2' options
# are specified.
$OSCAP xccdf eval --results $result --profile $p1 \
    --skip-rule $r1 --skip-rule $r2 $ds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"pass\"]"
:> $result

# Tests that all rules from profile P1 (profile contains only 4 rules) are
# evaluated when '--skip-rule' with an invalid rule ID is specified.
$OSCAP xccdf eval --results $result --profile $p1 --skip-rule $rx $ds 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"pass\"]"
:> $result

rm $result
