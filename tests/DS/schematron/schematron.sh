#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e -o pipefail

output="$(mktemp)"
$OSCAP xccdf validate --schematron "$srcdir/simple_ds.xml" >"$output" || ret=$?
[ $ret = 2 ]
grep -q "Schematron validation of OVAL Definition component 'test_single_rule.oval.xml': PASS" "$output"
grep -q "Schematron validation of XCCDF Checklist component 'scap_org.open-scap_cref_test_single_rule.xccdf.xml': FAIL" "$output"
grep -q "Global schematron validation using the source data stream schematron: PASS" $output
grep -q "Complete result of schematron validation of '$srcdir/simple_ds.xml': FAIL" $output
rm -f "$output"
