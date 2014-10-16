#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. ../../../test_common.sh

function test_fedora {
    local FEDORA_VERSION=$1
    local EXPECTED_NA=0

    local RPM=$(which rpm)
    if [ "x$?" != "x0" ]; then
        EXPECTED_NA=1
    else
        FEDORA_RELEASE=$(rpm -q fedora-release)
        if [ "x$?" != "x0" ]; then
            EXPECTED_NA=1
        else
            echo "$FEDORA_RELEASE" | grep -F "fedora-release-${FEDORA_VERSION}-"
            if [ "x$?" != "x0" ]; then
                EXPECTED_NA=1
            else
                EXPECTED_NA=0
            fi
        fi
    fi

    local INPUT=fedora${FEDORA_VERSION}-xccdf.xml

    local TMP_RESULTS=`mktemp`
    $OSCAP xccdf eval --results $TMP_RESULTS $srcdir/$INPUT
    if [ "x$?" != "x0" ]; then
        return 1
    fi

    local NOTAPPLICABLE_COUNT=$($XPATH $TMP_RESULTS 'count(//result[text()="notapplicable"])')
    rm -f $TMP_RESULTS

    if [ "$NOTAPPLICABLE_COUNT" == "$EXPECTED_NA" ]; then
        return 0
    fi

    return 1
}

function test_rhel {
    local RHEL_VERSION=$1
    local EXPECTED_NA=0

    local RPM=$(which rpm)
    if [ "x$?" != "x0" ]; then
        EXPECTED_NA=1
    else
        RHEL_RELEASE=$(rpm -q --whatprovides redhat-release)
        if [ "x$?" != "x0" ]; then
            EXPECTED_NA=1
        else
            if ! echo "$RHEL_RELEASE" | grep '^redhat-release'; then
                EXPECTED_NA=1
            elif rpm -q --queryformat "%{VERSION}" --whatprovides redhat-release | grep ${RHEL_VERSION}'\.9'; then
                # Workaround alpha and beta releases of Red Hat Enterprise Linux
                EXPECTED_NA=0
            elif echo "$RHEL_RELEASE" | grep "\.el${RHEL_VERSION}[._]"; then
                EXPECTED_NA=0
            elif [ "$RHEL_VERSION" == "5" ] && echo "$RHEL_RELEASE" | grep '^redhat-release-5'; then
                # On RHEL 5 the redhat-release package does not have %{RELEASE} defined.
                EXPECTED_NA=0
            else
                EXPECTED_NA=1
            fi
        fi
    fi

    local INPUT=rhel${RHEL_VERSION}-xccdf.xml

    local TMP_RESULTS=`mktemp`
    $OSCAP xccdf eval --results $TMP_RESULTS $srcdir/$INPUT
    if [ "x$?" != "x0" ]; then
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

test_init "test_api_xccdf_default_cpe.log"

test_run "test_api_xccdf_default_cpe_fedora16" test_fedora 16
test_run "test_api_xccdf_default_cpe_fedora17" test_fedora 17
test_run "test_api_xccdf_default_cpe_fedora18" test_fedora 18
test_run "test_api_xccdf_default_cpe_fedora19" test_fedora 19
test_run "test_api_xccdf_default_cpe_fedora20" test_fedora 20
test_run "test_api_xccdf_default_cpe_fedora21" test_fedora 21
test_run "test_api_xccdf_default_cpe_rhel5" test_rhel 5
test_run "test_api_xccdf_default_cpe_rhel6" test_rhel 6
test_run "test_api_xccdf_default_cpe_rhel7" test_rhel 7

test_exit
