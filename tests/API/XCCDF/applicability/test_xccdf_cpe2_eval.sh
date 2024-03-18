#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_api_xccdf_cpe2_eval {
    local INPUT=$srcdir/$1
    local CPE_DICT=$srcdir/$2
    local EXPECTED_NA=$3

    result=$(make_temp_file /tmp tmp)
    $OSCAP xccdf eval --cpe $CPE_DICT --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_NA '//result[text()="notapplicable"]'
    rm -f $result
}

# Testing.

test_init "test_api_xccdf_applicability.log"

test_run "test_api_xccdf_applicability_cpe2_applicable_rule" test_api_xccdf_cpe2_eval cpe2-applicable-rule-xccdf.xml cpe2-dict.xml 0

test_exit
