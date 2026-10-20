#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

stderr=$(mktemp -t ${name}.err.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)

ds="$srcdir/test_reference_ds.xml"
p1="xccdf_com.example.www_profile_P1"

# Test 1: --list-vars with --profile prints value IDs and resolved values
$OSCAP info --profile $p1 --list-vars $ds > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_value_V1	42" $stdout
grep -q "xccdf_com.example.www_value_V2	custom_val" $stdout
# Verify output contains exactly 2 lines
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout

# Test 2: --list-vars without --profile produces an error
$OSCAP info --list-vars $ds > $stdout 2> $stderr && exit 1 || true
grep -q "\-\-list-vars option requires \-\-profile" $stderr
:> $stdout
:> $stderr

# Test 3: --list-vars with --list-rules produces an error
$OSCAP info --profile $p1 --list-vars --list-rules $ds > $stdout 2> $stderr && exit 1 || true
grep -q "The \-\-list-rules and \-\-list-vars options can't be used at the same time." $stderr
:> $stdout
:> $stderr

# Test 4: --list-vars with standalone XCCDF tailoring file
tailoring="$srcdir/test_tailoring_file.xml"
tp="xccdf_com.example.www_profile_P1_tailored"
$OSCAP info --profile $tp --list-vars $tailoring > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
# V1 is overridden to 99 by tailoring, V2 is inherited from base profile
grep -q "xccdf_com.example.www_value_V1	99" $stdout
grep -q "xccdf_com.example.www_value_V2	custom_val" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout

# Test 5: --list-vars with SDS containing tailoring
ds_tailoring="$srcdir/test_reference_ds_with_tailoring.xml"
$OSCAP info --profile $tp --list-vars $ds_tailoring > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_value_V1	99" $stdout
grep -q "xccdf_com.example.www_value_V2	custom_val" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout

# Test 6: --list-vars with tailoring referencing SDS via file: prefix + cref
temp_dir="$(mktemp -d)"
cp "$srcdir/test_reference_ds.xml" $temp_dir
tailoring_sds="test_tailoring_file_sds_cref.xml"
sed "s;TEMP_DIRECTORY_PLACEHOLDER;$temp_dir;" "$srcdir/$tailoring_sds" > "$temp_dir/$tailoring_sds"
$OSCAP info --profile $tp --list-vars "$temp_dir/$tailoring_sds" > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_value_V1	99" $stdout
grep -q "xccdf_com.example.www_value_V2	custom_val" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout
rm -rf "$temp_dir"

# Test 7: --list-vars with tailoring referencing SDS via file: prefix, no cref
temp_dir="$(mktemp -d)"
cp "$srcdir/test_reference_ds.xml" $temp_dir
tailoring_sds_nf="test_tailoring_file_sds.xml"
sed "s;TEMP_DIRECTORY_PLACEHOLDER;$temp_dir;" "$srcdir/$tailoring_sds_nf" > "$temp_dir/$tailoring_sds_nf"
$OSCAP info --profile $tp --list-vars "$temp_dir/$tailoring_sds_nf" > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_value_V1	99" $stdout
grep -q "xccdf_com.example.www_value_V2	custom_val" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout
rm -rf "$temp_dir"

rm -f $stdout $stderr
