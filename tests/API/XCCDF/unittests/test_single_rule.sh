#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
prof1="xccdf_com.example.www_profile_test_single_rule"
prof2="xccdf_com.example.www_profile_test_single_rule_2"
rule_pass="xccdf_com.example.www_rule_test-pass"
rule_fail="xccdf_com.example.www_rule_test-fail"
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0
echo "Stderr file = $stderr"
echo "Result file = $result"


#### XCCDF test cases ####

# Tests that all rules from profile $prof1 (profile contains only 2 rules) are
# evaluated when '--rule' option is not specified.
$OSCAP xccdf eval --results $result --profile $prof1 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"pass\"]"
:> $result

# Tests that only selected passing rule from profile $prof1 is evaluated from
# XCCDF document.
$OSCAP xccdf eval --results $result --profile $prof1 \
	--rule $rule_pass $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"notselected\"]"
:> $result

# Tests that only selected rule from XCCDF document is evaluated without
# specifying a profile name -- this is possible as both rules in the XCCDF
# document are selected by default.
$OSCAP xccdf eval --results $result --rule $rule_pass \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"notselected\"]"
:> $result

# Tests <Value> in XCCDF document which should cause a failure in evaluation
# of the rule $rule_fail when profile $prof1 is not selected.
$OSCAP xccdf eval --results $result --rule $rule_fail \
	$srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"fail\"]"
:> $result

# Tests <Value> in XCCDF document with profile $prof1 selected which should
# cause that the rule $rule_fail will pass on its evaluation.
$OSCAP xccdf eval --results $result --profile $prof1 \
	--rule $rule_fail $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"notselected\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"pass\"]"
:> $result

# Tests evaluation of a rule which is notselected in profile $prof2.
$OSCAP xccdf eval --profile $prof2 \
	--rule $rule_pass $srcdir/${name}.xccdf.xml \
	2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

# Tests that error is printed when a non-existent rule is selected from XCCDF
# document.
$OSCAP xccdf eval --rule xccdf_non_existent $srcdir/${name}.xccdf.xml \
	2> $stderr || ret=$?
[ $ret -eq 1 ]
[ -f $stderr ]; [ -s $stderr ]; cat $stderr; :> $stderr


#### DS test cases ####

# Tests that only selected passing rule from profile $prof1 is evaluated from
# DS document.
$OSCAP xccdf eval --results $result --profile $prof1 \
	--rule $rule_pass $srcdir/${name}.ds.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"notselected\"]"
:> $result

# Tests that only selected passing rule from profile ${prof1}_customized from
# the tailoring file is evaluated (tailoring file was created using SCAP
# Workbench.
$OSCAP xccdf eval --tailoring-file $srcdir/${name}-tailoring.xml \
	--results $result --profile ${prof1}_customized \
	--rule $rule_pass $srcdir/${name}.ds.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate $result

assert_exists 1 "//rule-result[@idref=\"$rule_pass\"]/result[text()=\"pass\"]"
assert_exists 1 "//rule-result[@idref=\"$rule_fail\"]/result[text()=\"notselected\"]"
rm $result
