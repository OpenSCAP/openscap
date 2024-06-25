#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e -o pipefail

# both XCCDF and SDS schematrons find only warnings but not errors
output="$(mktemp)"
$OSCAP xccdf validate "$srcdir/simple_ds.xml" >"$output"
[ $? = 0 ]
grep -q "Schematron validation of OVAL Definition component 'test_single_rule.oval.xml': PASS" "$output"
grep -q "Schematron validation of XCCDF Checklist component 'scap_org.open-scap_cref_test_single_rule.xccdf.xml': PASS" "$output"
grep -q "Global schematron validation using the source data stream schematron: PASS" $output
grep -q "Complete result of schematron validation of '$srcdir/simple_ds.xml': PASS" $output
rm -f "$output"

# XCCDF schematron reports an error
output="$(mktemp)"
stderr="$(mktemp)"
$OSCAP xccdf validate "$srcdir/simple_ds_xccdf_schematron_error.xml" >"$output" 2>"$stderr" || ret=$?
[ $ret = 2 ]
grep -q "Error: The given @idref attribute 'xccdf_com.example.www_rule_test-pass2' must match a the @id or @cluster-id attributes of a 'Rule' or 'Group' element. See the XCCDF 1.2.1 specification, Section 6.5.3." $output
grep -q "Schematron validation of OVAL Definition component 'test_single_rule.oval.xml': PASS" "$output"
grep -q "Schematron validation of XCCDF Checklist component 'scap_org.open-scap_cref_test_single_rule.xccdf.xml': FAIL" "$output"
grep -q "Global schematron validation using the source data stream schematron: PASS" $output
grep -q "Complete result of schematron validation of '$srcdir/simple_ds_xccdf_schematron_error.xml': FAIL" $output
grep -q "OpenSCAP Error: Invalid SCAP Source Datastream (1.3) content in $srcdir/simple_ds_xccdf_schematron_error.xml" "$stderr"
rm -f "$output"
rm -f "$stderr"
