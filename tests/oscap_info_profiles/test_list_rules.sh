#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

stderr=$(mktemp -t ${name}.err.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)

ds="$srcdir/test_reference_ds.xml"
p1="xccdf_com.example.www_profile_P1"

# Test 1: --list-rules with --profile prints selected rule IDs
$OSCAP info --profile $p1 --list-rules $ds > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_rule_R1" $stdout
grep -q "xccdf_com.example.www_rule_R2" $stdout
grep -q "xccdf_com.example.www_rule_R3" $stdout
grep -q "xccdf_com.example.www_rule_R4" $stdout
# Verify output contains only rule IDs, one per line (4 rules = 4 lines)
[[ "$(wc -l < $stdout)" -eq 4 ]]
:> $stdout

# Test 2: --list-rules without --profile produces an error
$OSCAP info --list-rules $ds > $stdout 2> $stderr && exit 1 || true
grep -q "\-\-list-rules option requires \-\-profile" $stderr
:> $stdout
:> $stderr

# Test 3: --list-rules with standalone XCCDF tailoring file
tailoring="$srcdir/test_tailoring_file.xml"
tp="xccdf_com.example.www_profile_P1_tailored"
$OSCAP info --profile $tp --list-rules $tailoring > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_rule_R1" $stdout
grep -q "xccdf_com.example.www_rule_R2" $stdout
# R3 and R4 are deselected by tailoring
! grep -q "xccdf_com.example.www_rule_R3" $stdout
! grep -q "xccdf_com.example.www_rule_R4" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout

# Test 4: --list-rules with SDS containing tailoring
ds_tailoring="$srcdir/test_reference_ds_with_tailoring.xml"
$OSCAP info --profile $tp --list-rules $ds_tailoring > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_rule_R1" $stdout
grep -q "xccdf_com.example.www_rule_R2" $stdout
! grep -q "xccdf_com.example.www_rule_R3" $stdout
! grep -q "xccdf_com.example.www_rule_R4" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout

rm -f $stdout $stderr
