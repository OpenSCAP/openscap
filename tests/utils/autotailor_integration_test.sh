#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

autotailor="$top_srcdir/utils/autotailor"
tailoring="$(mktemp)"
ds="$srcdir/data_stream.xml"
json_tailoring="$srcdir/custom.json"
stdout="$(mktemp)"
original_profile="P1"
result="$(mktemp)"

# the original profile P1 selects rules R1 and R2

# select additional rule R3
python3 $autotailor --id-namespace "com.example.www" --select R3 $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'

# select additional rules R3, R4
python3 $autotailor --id-namespace "com.example.www" --select R3 --select R4 $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="pass"]'

# unselect rule R2
python3 $autotailor --id-namespace "com.example.www" --unselect R2 $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'

# unselect rule R2 and select R4
python3 $autotailor --id-namespace "com.example.www" --unselect R2 --select R4 $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="pass"]'

# select additional rule R3 and change its severity to high
python3 $autotailor --id-namespace "com.example.www" --select R3 --rule-severity R3=high $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1" and @severity="unknown"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2" and @severity="unknown"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3" and @severity="high"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4" and @severity="unknown"]'

# don't select rules, don't unselect rules, but change severity of all rules to high
python3 $autotailor --id-namespace "com.example.www" --rule-severity R1=high --rule-severity R2=high --rule-severity R3=high --rule-severity R4=high $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1" and @severity="high"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2" and @severity="high"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3" and @severity="high"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4" and @severity="high"]'


# select additional rule R4 and change its role to "unchecked"
python3 $autotailor --id-namespace "com.example.www" --select R4 --rule-role R4=unchecked $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1" and @role="full"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2" and @role="full"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3" and @role="full"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notchecked"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4" and @role="unchecked"]'


# select additional rule R3; the customized profile will have a special profile ID
customized_profile="xccdf_com.pink.elephant_profile_pineapple"
python3 $autotailor --new-profile-id $customized_profile --id-namespace "com.example.www" --select R3 $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile $customized_profile --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult[@id="xccdf_org.open-scap_testresult_'$customized_profile'"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'

# set value v1 to thirty
python3 $autotailor --id-namespace "com.example.www" --var-value V1=thirty $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/set-value[@idref="xccdf_com.example.www_value_V1" and text()="thirty"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'

# refine value v1 to 'thirty' (30) and v2 to 'other' (Other Value)
python3 $autotailor --id-namespace "com.example.www" --var-select V1=thirty --var-select V2=other $ds $original_profile > $tailoring
$OSCAP xccdf eval --profile P1_customized --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/set-value[@idref="xccdf_com.example.www_value_V1" and text()="30"]'
assert_exists 1 '/Benchmark/TestResult/set-value[@idref="xccdf_com.example.www_value_V2" and text()="Other Value"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'

# use JSON tailoring
python3 $autotailor $ds --id-namespace "com.example.www" --json-tailoring $json_tailoring > $tailoring
$OSCAP xccdf eval --profile JSON_P1 --progress --tailoring-file $tailoring --results $result $ds
assert_exists 1 '/Benchmark/TestResult/set-value[@idref="xccdf_com.example.www_value_V1" and text()="New Value"]'
assert_exists 1 '/Benchmark/TestResult/set-value[@idref="xccdf_com.example.www_value_V2" and text()="Some Value"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R1"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R2"]/result[text()="pass"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R3" and @severity="unknown"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4"]/result[text()="notselected"]'
assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_com.example.www_rule_R4" and @role="unchecked"]'
