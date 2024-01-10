#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)

ds="$srcdir/test_reference_ds.xml"
p1="xccdf_com.example.www_profile_P1"
r1="xccdf_com.example.www_rule_R1"
r2="xccdf_com.example.www_rule_R2"
r3="xccdf_com.example.www_rule_R3"
r4="xccdf_com.example.www_rule_R4"

# Tests if references are correctly shown in oscap info output
$OSCAP info --references $ds > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep -q "References:" $stdout
grep -q "animals: https://www.animals.com" $stdout
grep -q "fruit: https://www.fruit.com" $stdout
:> $stdout

# Tests that all rules from profile P1 (profile contains only 4 rules) are
# evaluated when '--reference' option is not specified.
$OSCAP xccdf eval --results $result --profile $p1 $ds > $stdout 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"pass\"]"
:> $stdout
:> $result

# Tests that rule R1 from profile P1 is evaluated when '--reference' option
# matches the rule R1.
$OSCAP xccdf eval --results $result --profile $p1 --reference "animals:3.14" $ds > $stdout 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"notselected\"]"
:> $stdout
:> $result

# Tests that rule R1 from profile P1 is evaluated when '--reference' option
# matches the rule R1. This test uses a different reference key than the
# previous test.
$OSCAP xccdf eval --results $result --profile $p1 --reference "fruit:42.42" $ds > $stdout 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"notselected\"]"
:> $stdout
:> $result

# Tests that only rules R2 and R3 from profile P1 are evaluated when
# '--reference' option matches the rule R2 and R3, both rules have
# the same reference item.
$OSCAP xccdf eval --results $result --profile $p1 --reference "animals:17.71.777" $ds > $stdout 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"notselected\"]"
:> $stdout
:> $result

# Tests that no rule from profile P1 is evaluated when '--reference' option
# doesn't match any reference in any rule.
$OSCAP xccdf eval --results $result --profile $p1 --reference "animals:99.66.33" $ds > $stdout 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"notselected\"]"
:> $stdout
:> $result

# Tests that when a wrong '--reference' option is provided OpenSCAP ignores it,
# evaluates all rules and prints a nice error messsage.
$OSCAP xccdf eval --results $result --profile $p1 --reference "aliens:XXX" $ds > $stdout 2> $stderr
grep -q "OpenSCAP Error: Reference type 'aliens' isn't available in this benchmark" $stderr
assert_exists 1 "//rule-result[@idref=\"$r1\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r2\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r3\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$r4\"]/result[text()=\"pass\"]"
:> $stdout
:> $result

# Tests that when a wrong '--reference' option with a valid name but missing
# identifier is provided OpenSCAP prints an errror message.
$OSCAP xccdf eval --results $result --profile $p1 --reference "animals" $ds > $stdout 2> $stderr || [[ $? -eq 1 ]]
grep -q "The --reference argument needs to be in form NAME:IDENTIFIER, using a colon as a separator." $stderr
:> $stdout
:> $result
