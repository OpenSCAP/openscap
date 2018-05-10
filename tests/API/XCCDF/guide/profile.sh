#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -o pipefail

. "$builddir/tests/test_common.sh"


function test_generate_guide_profile {
    local INPUT=$srcdir/$1
    local PROFILE=$2
    local EXPECTED_CONTENT=$3
    local UNEXPECTED_CONTENT=$4

    local GENERATED_CONTENT=$($OSCAP xccdf generate guide --profile $PROFILE "$INPUT")
    exit_code=0
    if [ "$?" != "0" ]; then
        return 1
    fi

    echo "$GENERATED_CONTENT" | grep "$EXPECTED_CONTENT"
    if [ "$?" != "0" ]; then
        echo "Generated content does not contain '$EXPECTED_CONTENT'!"
        echo "Generated content:"
        echo "$GENERATED_CONTENT"
        exit_code=1
    fi

    echo "$GENERATED_CONTENT" | grep "$UNEXPECTED_CONTENT"
    if [ "$?" == "0" ]; then
        echo "Generated content does contain '$UNEXPECTED_CONTENT', but it should not!"
        echo "Generated content:"
        echo "$GENERATED_CONTENT"
        exit_code=1
    fi

    return $exit_code
}


# Testing.

test_init "test_api_xccdf_guide_profile.log"

test_run "test_api_xccdf_guide_profile_value_selector_xccdf11" test_generate_guide_profile content-profile-xccdf11.xml default off_value bflmpsvz
test_run "test_api_xccdf_guide_profile_default_xccdf11" test_generate_guide_profile content-profile-xccdf11.xml default rule_1_title rule_2_title
test_run "test_api_xccdf_guide_profile_selects_xccdf11" test_generate_guide_profile content-profile-xccdf11.xml alternative rule_2_title rule_1_title

test_exit
