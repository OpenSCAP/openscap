#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_api_xccdf_embedded_cpe_eval {
    local INPUT=$srcdir/$1
    local EXPECTED_NA=$2

    result=$(make_temp_file /tmp tmp)
    $OSCAP xccdf eval --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_NA '//result[text()="notapplicable"]'
    rm -f $result
}
# Testing.

test_init "test_api_xccdf_applicability.log"

test_run "test_api_xccdf_applicability_cpe_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval applicable-rule-embedded-xccdf.xml 0
test_run "test_api_xccdf_applicability_cpe2_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-applicable-rule-embedded-xccdf.xml 0
test_run "test_api_xccdf_applicability_cpe2_negated_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-negated-applicable-rule-embedded-xccdf.xml 1
test_run "test_api_xccdf_applicability_cpe2_and_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-and-rule-embedded-xccdf.xml 2
test_run "test_api_xccdf_applicability_cpe2_or_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-or-rule-embedded-xccdf.xml 0

test_exit
