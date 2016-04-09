#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. ../../../test_common.sh

function test_api_xccdf_cpe_eval {
    local INPUT=$srcdir/$1
    local CPE_DICT=$srcdir/$2
    local EXPECTED_NA=$3

    result=`mktemp`
    $OSCAP xccdf eval --cpe $CPE_DICT --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_NA '//result[text()="notapplicable"]'
    rm -f $result
}

function test_api_xccdf_cpe2_eval {
    local INPUT=$srcdir/$1
    local CPE_DICT=$srcdir/$2
    local EXPECTED_NA=$3

    result=`mktemp`
    $OSCAP xccdf eval --cpe $CPE_DICT --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_NA '//result[text()="notapplicable"]'
    rm -f $result
}

function test_api_xccdf_embedded_cpe_eval {
    local INPUT=$srcdir/$1
    local EXPECTED_NA=$2

    result=`mktemp`
    $OSCAP xccdf eval --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_NA '//result[text()="notapplicable"]'
    rm -f $result
}
# Testing.

test_init "test_api_xccdf_applicability.log"

test_run "Populate TestResult/platform sub element" $srcdir/test_platform_element.sh
test_run "test_api_xccdf_applicability_cpe_applicable_rule" test_api_xccdf_cpe_eval applicable-rule-xccdf.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval applicable-rule-embedded-xccdf.xml 0
test_run "test_api_xccdf_applicability_cpe_applicable_benchmark" test_api_xccdf_cpe_eval applicable-benchmark-xccdf.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe_nonexistant_platforms_rule" test_api_xccdf_cpe_eval nonexistant-platforms-rule-xccdf.xml cpe-dict.xml 1
test_run "test_api_xccdf_applicability_cpe2_applicable_rule" test_api_xccdf_cpe2_eval cpe2-applicable-rule-xccdf.xml cpe2-dict.xml 0
test_run "test_api_xccdf_applicability_cpe2_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-applicable-rule-embedded-xccdf.xml 0
test_run "test_api_xccdf_applicability_cpe2_negated_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-negated-applicable-rule-embedded-xccdf.xml 1
test_run "test_api_xccdf_applicability_cpe2_and_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-and-rule-embedded-xccdf.xml 2
test_run "test_api_xccdf_applicability_cpe2_or_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-or-rule-embedded-xccdf.xml 0

test_run "test_api_xccdf_applicability_cpe2_applicable_embedded_rule_with_cpe_dict" test_api_xccdf_cpe_eval cpe2-applicable-rule-embedded-xccdf-combined.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe2_not_applicable_embedded_rule_with_cpe_dict" test_api_xccdf_cpe_eval cpe2-notapplicable-rule-embedded-xccdf-combined.xml cpe-dict.xml 1

test_run "xccdf:fix applicable and notapplicable" $srcdir/test_remediate_fix_notapplicable.sh
test_run "Complex selection of xccdf:fix based multiple factors" $srcdir/test_remediate_fix_processing.sh
test_run "Complex selection of xccdf:fix + DataStream" $srcdir/test_remediate_fix_processing_ds.sh
test_run "Generate fix just as the Anaconda does + CPE" $srcdir/test_report_anaconda_fixes.sh

test_exit
