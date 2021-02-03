#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e -o pipefail

# Test a signed SCAP source data stream with a valid signature
stdout=$(mktemp)
stderr=$(mktemp)
verbose=$(mktemp)
result=$(mktemp)
$OSCAP xccdf eval --verbose INFO --verbose-log-file $verbose --results-arf $result $srcdir/simple_ds_valid_sign.xml >$stdout 2>$stderr
! [ -s $stderr ]
grep -q "Validating XML signature" $verbose
grep -q "Signature is OK" $verbose
grep -q "SignedInfo references (ok/all): 3/3" $verbose
grep -q "Manifests references (ok/all): 2/2" $verbose
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_com.example.www_rule_test-pass"]/result[text()="pass"]'
rm -f $stdout
rm -f $stderr
rm -f $verbose

# Test a signed SCAP source data stream with an invalid signature
stdout=$(mktemp)
stderr=$(mktemp)
verbose=$(mktemp)
result=$(mktemp)
$OSCAP xccdf eval --verbose INFO --verbose-log-file $verbose --results-arf $result $srcdir/simple_ds_invalid_sign.xml >$stdout 2>$stderr || ret=$?
[ $ret = 1 ]
[ -s $stderr ]
! [ -s $arf ]
grep -q "OpenSCAP Error: Invalid signature in SCAP Source Datastream (1.3) content in $srcdir/simple_ds_invalid_sign.xml" $stderr
grep -q "Validating XML signature" $verbose
grep -q "Signature is invalid" $verbose
grep -q "SignedInfo references (ok/all): 3/3" $verbose
grep -q "Manifests references (ok/all): 2/2" $verbose
rm -f $stdout
rm -f $stderr
rm -f $verbose

# Test skipping signature validation on a signed SCAP source data stream with an invalid signature
stdout=$(mktemp)
stderr=$(mktemp)
verbose=$(mktemp)
result=$(mktemp)
$OSCAP xccdf eval --skip-signature-validation --results-arf $result $srcdir/simple_ds_invalid_sign.xml >$stdout 2>$stderr
assert_exists 1 '//TestResult/rule-result[@idref="xccdf_com.example.www_rule_test-pass"]/result[text()="pass"]'
rm -f $stdout
rm -f $stderr
rm -f $verbose

# Test a signed SCAP source data stream with modified SCAP content
stdout=$(mktemp)
stderr=$(mktemp)
verbose=$(mktemp)
result=$(mktemp)
$OSCAP xccdf eval --verbose INFO --verbose-log-file $verbose --results-arf $result $srcdir/simple_ds_modified.xml >$stdout 2>$stderr || ret=$?
[ $ret = 1 ]
[ -s $stderr ]
! [ -s $arf ]
grep -q "OpenSCAP Error: Invalid signature in SCAP Source Datastream (1.3) content in $srcdir/simple_ds_modified.xml" $stderr
grep -q "Validating XML signature" $verbose
grep -q "Signature is OK" $verbose
grep -q "SignedInfo references (ok/all): 3/3" $verbose
grep -q "Manifests references (ok/all): 1/2" $verbose
rm -f $stdout
rm -f $stderr
rm -f $verbose
