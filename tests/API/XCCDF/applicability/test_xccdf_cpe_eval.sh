#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_api_xccdf_cpe_eval {
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

test_run "test_api_xccdf_applicability_cpe_applicable_rule" test_api_xccdf_cpe_eval applicable-rule-xccdf.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe_applicable_benchmark" test_api_xccdf_cpe_eval applicable-benchmark-xccdf.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe_nonexistant_platforms_rule" test_api_xccdf_cpe_eval nonexistant-platforms-rule-xccdf.xml cpe-dict.xml 1
test_run "test_api_xccdf_applicability_cpe2_applicable_embedded_rule_with_cpe_dict" test_api_xccdf_cpe_eval cpe2-applicable-rule-embedded-xccdf-combined.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe2_not_applicable_embedded_rule_with_cpe_dict" test_api_xccdf_cpe_eval cpe2-notapplicable-rule-embedded-xccdf-combined.xml cpe-dict.xml 1

test_exit
