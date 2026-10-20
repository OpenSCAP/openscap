#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_generate_guide {
    local INPUT=$srcdir/$1
    local EXPECTED_CONTENT=$2

    local GENERATED_CONTENT=$($OSCAP xccdf generate guide "$INPUT")
    if [ "$?" != "0" ]; then
        return 1
    fi

    echo "$GENERATED_CONTENT" | grep "$EXPECTED_CONTENT"
    if [ "$?" == "0" ]; then
        return 0
    fi

    echo "Generated content does not contain '$EXPECTED_CONTENT'!"
    echo "Generated content:"
    echo "$GENERATED_CONTENT"

    return 1
}


# Testing.

test_init "test_api_xccdf_guide.log"

test_run "test_api_xccdf_guide_xccdf11" test_generate_guide content-xccdf11.xml xccdf_moc.elpmaxe.www_rule_1
test_run "test_api_xccdf_guide_xccdf12" test_generate_guide content-xccdf12.xml xccdf_moc.elpmaxe.www_rule_1
test_run "test_api_xccdf_guide_value_default_xccdf11" test_generate_guide content-profile-xccdf11.xml off_default

test_exit
