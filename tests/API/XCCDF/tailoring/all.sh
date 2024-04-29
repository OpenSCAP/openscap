#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_api_xccdf_tailoring {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2
    local PROFILE=$3
    local EXPECTED_PASS=$4

    result=`mktemp`
    $OSCAP xccdf eval --tailoring-file $TAILORING --profile $PROFILE --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_PASS '//result[text()="pass"]'
    rm -f $result
}

function test_api_xccdf_tailoring_ds {
    local INPUT=$srcdir/$1
    local TAILORING_ID=$2
    local PROFILE=$3
    local EXPECTED_PASS=$4

    result=`mktemp`
    $OSCAP xccdf eval --tailoring-id $TAILORING_ID --profile $PROFILE --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_PASS '//result[text()="pass"]'
    rm -f $result
}

function test_api_xccdf_tailoring_ds_hybrid {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2
    local PROFILE=$3
    local EXPECTED_PASS=$4

    result=`mktemp`
    $OSCAP xccdf eval --tailoring-file $TAILORING --profile $PROFILE --results $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_PASS '//result[text()="pass"]'
    rm -f $result
}

function test_api_xccdf_tailoring_oscap_info {
    local INPUT=$srcdir/$1

    local INFO_RESULTS=`$OSCAP info $INPUT`
    if [ "$?" != "0" ]; then
        return 1
    fi

    echo "$INFO_RESULTS" | grep "XCCDF Tailoring"
    if [ "$?" != "0" ]; then
        return 1
    fi
}

function test_api_xccdf_tailoring_autonegotiation {
    local TAILORING=$srcdir/$1
    local PROFILE=$2
    local EXPECTED_PASS=$3

    result=`mktemp`
    $OSCAP xccdf eval --profile $PROFILE --results $result $TAILORING
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists $EXPECTED_PASS '//result[text()="pass"]'
    rm -f $result
}

function test_api_xccdf_tailoring_simple_include_in_arf {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2

    result=`mktemp`
    $OSCAP xccdf eval --tailoring-file $TAILORING --results-arf $result $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    assert_exists 1 '/arf:asset-report-collection/arf:report-requests/arf:report-request/arf:content/ds:data-stream-collection/ds:component/Tailoring'
    rm -f $result
}

function test_api_xccdf_tailoring_simple_include_in_arf_xlink_namespace {
    # This test case is a regression test for RHEL-34104

    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2

    result=`mktemp`
    stderr=`mktemp`
    $OSCAP xccdf eval --tailoring-file $TAILORING --results-arf $result $INPUT 2>"$stderr"
    if [ "$?" != "0" ]; then
        return 1
    fi

    [ ! -s "$stderr" ]
    assert_exists 1 '/arf:asset-report-collection/arf:report-requests/arf:report-request/arf:content/ds:data-stream-collection/ds:component/Tailoring'

    rm -f "$result"
    rm -f "$stderr"
}

function test_api_xccdf_tailoring_profile_include_in_arf {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2

    result=`mktemp`
    # "xccdf_com.example.www_profile_customized" customizes "xccdf_com.example.www_profile_baseline1"
    $OSCAP xccdf eval --tailoring-file $TAILORING --profile "xccdf_com.example.www_profile_customized" --results-arf $result $INPUT || [ "$?" == "2" ]

    component_xpath='/arf:asset-report-collection/arf:report-requests/arf:report-request/arf:content/ds:data-stream-collection/ds:component'
    assert_exists 3 $component_xpath
    assert_exists 3 $component_xpath'/@timestamp'
    assert_exists 1 $component_xpath'/xccdf:Tailoring'
    assert_exists 1 $component_xpath'/xccdf:Tailoring/xccdf:Profile'
    assert_exists 1 $component_xpath'/xccdf:Tailoring/xccdf:Profile[@id="xccdf_com.example.www_profile_customized"]'
    assert_exists 1 $component_xpath'/xccdf:Tailoring/xccdf:Profile[@extends="xccdf_com.example.www_profile_baseline1"]'
    rm -f $result
}

function test_api_xccdf_tailoring_profile_generate_fix {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2

    tailoring_result=`mktemp`
    fix_result=`mktemp`
    # tailoring profile only with "always fail" rule and generate bash fix
    $OSCAP xccdf eval --tailoring-file $TAILORING --profile "xccdf_com.example.www_profile_customized" --results-arf $tailoring_result $INPUT || [ "$?" == "2" ]
    tailoring_id=$($XPATH $tailoring_result 'string(//ds:component-ref[contains(@id, "_tailoring")]/@id)')
    $OSCAP xccdf generate fix --tailoring-id $tailoring_id --result-id xccdf_org.open-scap_test-result_xccdf-com.example.www_profile_customized --results $fix_result $tailoring_result

    if ! grep -q "echo \"Fix the first rule\"" $fix_result; then
        return 1
    fi

    rm -f $tailoring_result $fix_result
}

function test_api_xccdf_tailoring_profile_generate_guide {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2

    guide=`mktemp`
    # tailoring profile only with "always fail" rule and generate HTML guide
    $OSCAP xccdf generate guide --tailoring-file $TAILORING --profile "xccdf_com.example.www_profile_customized" --output $guide $INPUT

    grep -q "Baseline Testing Profile 1 \[CUSTOMIZED\]" $guide
    # profile 'customized' selects first rule and deselects the second
    grep -q "xccdf_com.example.www_rule_first" $guide
    grep -v "xccdf_com.example.www_rule_second" $guide
    rm -f $guide
}

# Testing.

test_init "test_api_xccdf_tailoring.log"

test_run "test_api_xccdf_tailoring_default" test_api_xccdf_tailoring simple-xccdf.xml simple-tailoring.xml xccdf_org.open-scap_profile_default 1
test_run "test_api_xccdf_tailoring_unselecting" test_api_xccdf_tailoring simple-xccdf.xml simple-tailoring.xml xccdf_org.open-scap_profile_unselecting 0
test_run "test_api_xccdf_tailoring_override" test_api_xccdf_tailoring simple-xccdf.xml simple-tailoring.xml xccdf_org.open-scap_profile_override 1
test_run "test_api_xccdf_tailoring_unselecting_noop" test_api_xccdf_tailoring simple-xccdf.xml simple-tailoring.xml xccdf_org.open-scap_profile_unselecting_noop 0
test_run "test_api_xccdf_tailoring_ds_default" test_api_xccdf_tailoring_ds simple-ds.xml scap_org.open-scap_cref_simple-tailoring.xml xccdf_org.open-scap_profile_default 1
test_run "test_api_xccdf_tailoring_ds_unselecting" test_api_xccdf_tailoring_ds simple-ds.xml scap_org.open-scap_cref_simple-tailoring.xml xccdf_org.open-scap_profile_unselecting 0
test_run "test_api_xccdf_tailoring_ds_override" test_api_xccdf_tailoring_ds simple-ds.xml scap_org.open-scap_cref_simple-tailoring.xml xccdf_org.open-scap_profile_override 1
test_run "test_api_xccdf_tailoring_ds_hybrid_default" test_api_xccdf_tailoring_ds_hybrid simple-ds.xml simple-tailoring.xml xccdf_org.open-scap_profile_default 1
test_run "test_api_xccdf_tailoring_ds_hybrid_unselecting" test_api_xccdf_tailoring_ds_hybrid simple-ds.xml simple-tailoring.xml xccdf_org.open-scap_profile_unselecting 0
test_run "test_api_xccdf_tailoring_ds_hybrid_override" test_api_xccdf_tailoring_ds_hybrid simple-ds.xml simple-tailoring.xml xccdf_org.open-scap_profile_override 1
test_run "test_api_xccdf_tailoring_oscap_info_11" test_api_xccdf_tailoring_oscap_info simple-tailoring11.xml 1
test_run "test_api_xccdf_tailoring_oscap_info_12" test_api_xccdf_tailoring_oscap_info simple-tailoring.xml 1
test_run "test_api_xccdf_tailoring_autonegotiation" test_api_xccdf_tailoring_autonegotiation simple-tailoring-autonegotiation.xml xccdf_org.open-scap_profile_default 1
test_run "test_api_xccdf_tailoring_simple_include_in_arf" test_api_xccdf_tailoring_simple_include_in_arf simple-xccdf.xml simple-tailoring.xml
test_run "test_api_xccdf_tailoring_simple_include_in_arf_xlink_namespace" test_api_xccdf_tailoring_simple_include_in_arf_xlink_namespace xlink-test-simple-ds.xml simple-tailoring.xml
test_run "test_api_xccdf_tailoring_profile_include_in_arf" test_api_xccdf_tailoring_profile_include_in_arf baseline.xccdf.xml baseline.tailoring.xml
test_run "test_api_xccdf_tailoring_profile_generate_fix" test_api_xccdf_tailoring_profile_generate_fix baseline.xccdf.xml baseline.tailoring.xml
test_run "test_api_xccdf_tailoring_profile_generate_guide" test_api_xccdf_tailoring_profile_generate_guide baseline.xccdf.xml baseline.tailoring.xml


test_exit
