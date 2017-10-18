#!/bin/bash

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_validation {
    local TYPE=$1
    local FILE=${srcdir}/$2
    local EXPECTED_CODE=$3
	local ret=0
	$OSCAP ds $TYPE-validate $FILE || ret=$?
	if [ $ret -eq $EXPECTED_CODE ]; then
		return 0
	fi
	return 1
}

test_init test_validation.log
test_run "valid-sds" test_validation sds sds-valid.xml 0
test_run "invalid-sds" test_validation sds sds-invalid.xml 1
test_run "invalid-xccdf-sds" test_validation sds sds-invalid-xccdf.xml 1
test_run "invalid-oval-sds" test_validation sds sds-invalid-oval.xml 1

test_run "valid-rds" test_validation rds rds-valid.xml 0
test_run "invalid-rds" test_validation rds rds-invalid.xml 1
test_exit
