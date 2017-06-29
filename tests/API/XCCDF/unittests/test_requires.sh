#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
prof1="xccdf_com.example.www_profile_test_requires_1"
prof2="xccdf_com.example.www_profile_test_requires_2"
prof3="xccdf_com.example.www_profile_test_requires_3"
prof4="xccdf_com.example.www_profile_test_requires_4"
prof5="xccdf_com.example.www_profile_test_requires_5"
prof6="xccdf_com.example.www_profile_test_requires_6"
prof7="xccdf_com.example.www_profile_test_requires_7"
prof8="xccdf_com.example.www_profile_test_requires_8"
prof9="xccdf_com.example.www_profile_test_requires_9"
prof10="xccdf_com.example.www_profile_test_requires_10"
rule_pass="xccdf_com.example.www_rule_test-pass"
rule_fail="xccdf_com.example.www_rule_test-fail"
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0
echo "Stderr file = $stderr"
echo "Result file = $result"


#### XCCDF test cases ####

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof1 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 4 "//rule-result/result[text()=\"pass\"]"
assert_exists 6 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"
:> $result

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof2 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 3 "//rule-result/result[text()=\"pass\"]"
assert_exists 7 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof3 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 2 "//rule-result/result[text()=\"pass\"]"
assert_exists 8 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof4 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 0 "//rule-result/result[text()=\"pass\"]"
assert_exists 10 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof5 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 2 "//rule-result/result[text()=\"pass\"]"
assert_exists 8 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof6 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 3 "//rule-result/result[text()=\"pass\"]"
assert_exists 7 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof7 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 3 "//rule-result/result[text()=\"pass\"]"
assert_exists 7 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof8 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 1 "//rule-result/result[text()=\"pass\"]"
assert_exists 9 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof9 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 3 "//rule-result/result[text()=\"pass\"]"
assert_exists 7 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

# Tests that all rules from profile $prof1 are selected and evaluated when
$OSCAP xccdf eval --results $result --profile $prof10 \
	$srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

$OSCAP xccdf validate-xml $result

assert_exists 2 "//rule-result/result[text()=\"pass\"]"
assert_exists 8 "//rule-result/result[text()=\"notselected\"]"
assert_exists 0 "//rule-result/result[text()=\"fail\"]"

rm $result
