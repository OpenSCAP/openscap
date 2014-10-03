#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $srcdir/../../../test_common.sh

#set -e -o pipefail

function test_api_xccdf_tailoring {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2
    local PROFILE=$3
    local EXPECTED_PASS=$4

    local TMP_RESULTS=`mktemp`
    $OSCAP xccdf eval --tailoring-file $TAILORING --profile $PROFILE --results $TMP_RESULTS $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    local PASS_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="pass"])')
    rm -f $TMP_RESULTS

    if [ "$PASS_COUNT" == "$EXPECTED_PASS" ]; then
        return 0
    fi

    return 1
}

function test_api_xccdf_tailoring_ds {
    local INPUT=$srcdir/$1
    local TAILORING_ID=$2
    local PROFILE=$3
    local EXPECTED_PASS=$4

    local TMP_RESULTS=`mktemp`
    $OSCAP xccdf eval --tailoring-id $TAILORING_ID --profile $PROFILE --results $TMP_RESULTS $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    local PASS_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="pass"])')
    rm -f $TMP_RESULTS

    if [ "$PASS_COUNT" == "$EXPECTED_PASS" ]; then
        return 0
    fi

    return 1
}

function test_api_xccdf_tailoring_ds_hybrid {
    local INPUT=$srcdir/$1
    local TAILORING=$srcdir/$2
    local PROFILE=$3
    local EXPECTED_PASS=$4

    local TMP_RESULTS=`mktemp`
    $OSCAP xccdf eval --tailoring-file $TAILORING --profile $PROFILE --results $TMP_RESULTS $INPUT
    if [ "$?" != "0" ]; then
        return 1
    fi

    local PASS_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="pass"])')
    rm -f $TMP_RESULTS

    if [ "$PASS_COUNT" == "$EXPECTED_PASS" ]; then
        return 0
    fi

    return 1
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

    local TMP_RESULTS=`mktemp`
    $OSCAP xccdf eval --profile $PROFILE --results $TMP_RESULTS $TAILORING
    if [ "$?" != "0" ]; then
        return 1
    fi

    local PASS_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="pass"])')
    rm -f $TMP_RESULTS

    if [ "$PASS_COUNT" == "$EXPECTED_PASS" ]; then
        return 0
    fi

    return 1
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

test_exit
