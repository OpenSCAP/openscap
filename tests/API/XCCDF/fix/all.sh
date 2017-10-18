#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_generate_fix {
    local INPUT=$srcdir/$1
    local TESTRESULT_ID=$2
    local EXPECTED_FIX=$3

    # grep to strip out whitespace and comments
    # `tail -n +2` to skip the first line with progress reporting
    local GENERATED_FIX=$($OSCAP xccdf generate fix --result-id "$TESTRESULT_ID" "$INPUT" | grep -v -E "^([\t ]*|[\t ]*#.*)$" | tail -n +2)
    if [ "$?" != "0" ]; then
        return 1
    fi
    echo "$GENERATED_FIX"

    if [ "$GENERATED_FIX" == "$EXPECTED_FIX" ]; then
        return 0
    fi

    echo "Generated fix doesn't match expected fix!"
    echo "'$GENERATED_FIX' != '$EXPECTED_FIX'"

    return 1
}

# Testing.

test_init "test_api_xccdf_fix.log"

test_run "test_api_xccdf_fix_xccdf11" test_generate_fix results-xccdf11.xml xccdf_org.open-scap_testresult_default-profile echo
test_run "test_api_xccdf_fix_xccdf12" test_generate_fix results-xccdf12.xml xccdf_org.open-scap_testresult_default-profile echo

test_exit
