#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. ../../../test_common.sh

function test_progress {
    local INPUT=$srcdir/$1
    local EXPECTED_CONTENT=$2

    local GENERATED_CONTENT=$($OSCAP xccdf eval --progress "$INPUT")
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

test_init "test_api_xccdf_progress.log"

test_run "test_api_xccdf_progress_xccdf11" test_progress content-xccdf11.xml xccdf_moc.elpmaxe.www_rule_1:pass
test_run "test_api_xccdf_progress_xccdf12" test_progress content-xccdf12.xml xccdf_moc.elpmaxe.www_rule_1:pass

test_exit
