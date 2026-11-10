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

# Test 5: --list-rules with tailoring referencing SDS via file: prefix + cref
temp_dir="$(mktemp -d)"
cp "$srcdir/test_reference_ds.xml" $temp_dir
tailoring_sds="test_tailoring_file_sds_cref.xml"
sed "s;TEMP_DIRECTORY_PLACEHOLDER;$temp_dir;" "$srcdir/$tailoring_sds" > "$temp_dir/$tailoring_sds"
$OSCAP info --profile $tp --list-rules "$temp_dir/$tailoring_sds" > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_rule_R1" $stdout
grep -q "xccdf_com.example.www_rule_R2" $stdout
! grep -q "xccdf_com.example.www_rule_R3" $stdout
! grep -q "xccdf_com.example.www_rule_R4" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout
rm -rf "$temp_dir"

# Test 6: --list-rules with tailoring referencing SDS via file: prefix, no cref
temp_dir="$(mktemp -d)"
cp "$srcdir/test_reference_ds.xml" $temp_dir
tailoring_sds_nf="test_tailoring_file_sds.xml"
sed "s;TEMP_DIRECTORY_PLACEHOLDER;$temp_dir;" "$srcdir/$tailoring_sds_nf" > "$temp_dir/$tailoring_sds_nf"
$OSCAP info --profile $tp --list-rules "$temp_dir/$tailoring_sds_nf" > $stdout 2> $stderr
[[ -f $stderr ]]; [[ ! -s $stderr ]]; :> $stderr
grep -q "xccdf_com.example.www_rule_R1" $stdout
grep -q "xccdf_com.example.www_rule_R2" $stdout
! grep -q "xccdf_com.example.www_rule_R3" $stdout
! grep -q "xccdf_com.example.www_rule_R4" $stdout
[[ "$(wc -l < $stdout)" -eq 2 ]]
:> $stdout
rm -rf "$temp_dir"

# Test 7: --list-rules with tailoring referencing SDS via file: prefix, no cref,
#         providing the original profile instead of the customized one
temp_dir="$(mktemp -d)"
cp "$srcdir/test_reference_ds.xml" $temp_dir
tailoring_sds_nf="test_tailoring_file_sds.xml"
sed "s;TEMP_DIRECTORY_PLACEHOLDER;$temp_dir;" "$srcdir/$tailoring_sds_nf" > "$temp_dir/$tailoring_sds_nf"
$OSCAP info --profile "xccdf_com.example.www_profile_P1" --list-rules "$temp_dir/$tailoring_sds_nf" > $stdout 2> $stderr
[[ -f $stdout ]]; [[ ! -s $stdout ]]; :> $stdout
grep -q "Profile 'xccdf_com.example.www_profile_P1' not found in the tailoring file. Provide the customized profile ID. Get available profiles using:" "$stderr"
grep -q "$ oscap info $temp_dir/$tailoring_sds_nf" "$stderr"
[[ "$(wc -l < $stderr)" -eq 2 ]]
:> $stderr
rm -rf "$temp_dir"

rm -f $stdout $stderr

# --profiles must produce only ID:title rows for standalone tailoring files.
stdout=$(mktemp -t info_profiles.out.XXXXXX)
stderr=$(mktemp -t info_profiles.err.XXXXXX)
tailoring=$(mktemp -t info_profiles.xml.XXXXXX)
expected=$(mktemp -t info_profiles.expected.XXXXXX)
trap 'rm -f "$stdout" "$stderr" "$tailoring" "$expected"' EXIT

$OSCAP info --profiles "$srcdir/test_tailoring_file.xml" >"$stdout" 2>"$stderr"
[ ! -s "$stderr" ]
printf '%s\n' 'xccdf_com.example.www_profile_P1_tailored:Tailored P1' >"$expected"
diff -u "$expected" "$stdout"

# An unavailable benchmark must not prevent listing locally defined profiles.
cat >"$tailoring" <<'XML'
<Tailoring xmlns="http://checklists.nist.gov/xccdf/1.2" id="xccdf_com.example.www_tailoring_profiles">
  <benchmark href="missing-benchmark.xml"/>
  <version>1.0</version>
  <Profile id="xccdf_com.example.www_profile_first" extends="xccdf_com.example.www_profile_base">
    <title>First
profile</title>
  </Profile>
  <Profile id="xccdf_com.example.www_profile_untitled" extends="xccdf_com.example.www_profile_base"/>
  <Profile id="xccdf_com.example.www_profile_abstract" abstract="true"><title>Abstract profile</title></Profile>
</Tailoring>
XML
$OSCAP info --profiles "$tailoring" >"$stdout" 2>"$stderr"
[ ! -s "$stderr" ]
cat >"$expected" <<'ROWS'
xccdf_com.example.www_profile_first:Firstprofile
xccdf_com.example.www_profile_untitled:
xccdf_com.example.www_profile_abstract:Abstract profile
ROWS
diff -u "$expected" "$stdout"

# Normal info retains its document header, benchmark hint and verbose profiles.
$OSCAP info "$tailoring" >"$stdout" 2>"$stderr"
[ ! -s "$stderr" ]
grep -Fx 'Document type: XCCDF Tailoring' "$stdout"
grep -Fx 'Benchmark Hint: missing-benchmark.xml' "$stdout"
grep -Fx 'Profiles:' "$stdout"

# SDS tailoring profiles use the same terse output without a benchmark hint.
$OSCAP info --profiles "$srcdir/test_reference_ds_with_tailoring.xml" >"$stdout" 2>"$stderr"
[ ! -s "$stderr" ]
! grep -q 'Benchmark Hint:' "$stdout"
grep -Fx 'xccdf_com.example.www_profile_P1_tailored:Tailored P1' "$stdout"
! grep -vE '^xccdf_[^:]+:.*$' "$stdout"

# Tailoring without local profiles has an empty terse listing, not headers.
cat >"$tailoring" <<'XML'
<Tailoring xmlns="http://checklists.nist.gov/xccdf/1.2" id="xccdf_com.example.www_tailoring_empty">
  <benchmark href="missing-benchmark.xml"/>
  <version>1.0</version>
</Tailoring>
XML
$OSCAP info --profiles "$tailoring" >"$stdout" 2>"$stderr"
[ ! -s "$stderr" ]
[ ! -s "$stdout" ]
