#!/usr/bin/env bash

# Regression test for rhbz#1945580

. $builddir/tests/test_common.sh

set -e -o pipefail

result=$(mktemp)
stderr=$(mktemp)

[ ! -e "/tmp/test_variable_conversion.txt" ] && rm -f "/tmp/test_variable_conversion.txt"
echo "auth      required         pam_faillock.so preauth silent even_deny_root deny=3 fail_interval=900 unlock_time=never fail_interval=900 deny=3"  >  "/tmp/test_variable_conversion.txt"

$OSCAP oval eval --results $result $srcdir/test_variable_conversion.oval.xml 2> $stderr
[ "$?" -eq 0 ]

grep -Pq "W: oscap:\s+Conversion of the string \"never\" to an integer \(\d+ bits\) failed: Invalid argument" "$stderr"
grep -Pq "W: oscap:\s+Can't compare variable 'oval:ssg-var_accounts_passwords_pam_faillock_unlock_time:var:1' value = '0' with collected item entity = 'never'" "$stderr"
! grep -Pq "OpenSCAP Error:\s+Conversion of the string \"never\" to an integer \(\d+ bits\) failed: Invalid argument" "$stderr"
! grep -Pq "^E: oscap:\s+Error occured when comparing a variable 'oval:ssg-var_accounts_passwords_pam_faillock_unlock_time:var:1' value '0' with collected item entity = 'never'" "$stderr"

assert_exists 1 '/oval_results'
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="true"]'
assert_exists 3 '/oval_results/results/system/tests/test/tested_item'

rm -f "/tmp/test_variable_conversion.txt"
rm -f $result $stderr
