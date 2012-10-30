#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $srcdir/../../../test_common.sh

function test_api_xccdf_cpe_eval {
    local INPUT=$srcdir/$1
    local CPE_DICT=$srcdir/$2
    local EXPECTED_NA=$3
    
    local OSCAP_DIR=`cd ../../../../utils/.libs; pwd`

    local TMP_RESULTS=`mktemp`
    $OSCAP_DIR/oscap xccdf eval --cpe-dict $CPE_DICT --results $TMP_RESULTS $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    local NOTAPPLICABLE_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="notapplicable"])')
    rm -f $TMP_RESULTS

    if [ "$NOTAPPLICABLE_COUNT" == "$EXPECTED_NA" ]; then
        return 0
    fi

    return 1
}

function test_api_xccdf_cpe2_eval {
    local INPUT=$srcdir/$1
    local CPE_DICT=$srcdir/$2
    local EXPECTED_NA=$3
    
    local OSCAP_DIR=`cd ../../../../utils/.libs; pwd`

    local TMP_RESULTS=`mktemp`
    $OSCAP_DIR/oscap xccdf eval --cpe2-dict $CPE_DICT --results $TMP_RESULTS $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    local NOTAPPLICABLE_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="notapplicable"])')
    rm -f $TMP_RESULTS

    if [ "$NOTAPPLICABLE_COUNT" == "$EXPECTED_NA" ]; then
        return 0
    fi

    return 1
}

function test_api_xccdf_embedded_cpe_eval {
    local INPUT=$srcdir/$1
    local EXPECTED_NA=$2
    
    local OSCAP_DIR=`cd ../../../../utils/.libs; pwd`

    local TMP_RESULTS=`mktemp`
    $OSCAP_DIR/oscap xccdf eval --results $TMP_RESULTS $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    local NOTAPPLICABLE_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="notapplicable"])')
    rm -f $TMP_RESULTS

    if [ "$NOTAPPLICABLE_COUNT" == "$EXPECTED_NA" ]; then
        return 0
    fi

    return 1
}
# Testing.

test_init "test_api_xccdf_applicability.log"

test_run "test_api_xccdf_applicability_cpe_applicable_rule" test_api_xccdf_cpe_eval applicable-rule-xccdf.xml cpe-dict.xml 0
test_run "test_api_xccdf_applicability_cpe_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval applicable-rule-embedded-xccdf.xml 0
test_run "test_api_xccdf_applicability_cpe_nonexistant_platforms_rule" test_api_xccdf_cpe_eval nonexistant-platforms-rule-xccdf.xml cpe-dict.xml 1
test_run "test_api_xccdf_applicability_cpe2_applicable_rule" test_api_xccdf_cpe2_eval cpe2-applicable-rule-xccdf.xml cpe2-dict.xml 0
test_run "test_api_xccdf_applicability_cpe2_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-applicable-rule-embedded-xccdf.xml 0
test_run "test_api_xccdf_applicability_cpe2_negated_applicable_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-negated-applicable-rule-embedded-xccdf.xml 1
test_run "test_api_xccdf_applicability_cpe2_and_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-and-rule-embedded-xccdf.xml 2
test_run "test_api_xccdf_applicability_cpe2_or_embedded_rule" test_api_xccdf_embedded_cpe_eval cpe2-or-rule-embedded-xccdf.xml 0

test_exit
