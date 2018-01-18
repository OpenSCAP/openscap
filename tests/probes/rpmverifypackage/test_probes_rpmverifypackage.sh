#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Probes Test Suite.
# Tests the rpmverifypackage probe.
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail
set -x

function test_probes_rpmverifypackage {

    probecheck "rpmverifypackage" || return 255
    require "rpm" || return 255
    DF="test_probes_rpmverifypackage.xml"
    RF="results.xml"

    RPM_NAME=$1
    RPM_EPOCH=`rpm --qf "%{EPOCH}\n" -q $RPM_NAME | head -1`
    RPM_ARCH=`rpm --qf "%{ARCH}\n" -q $RPM_NAME | head -1`
    RPM_VERSION=`rpm --qf "%{VERSION}\n" -q $RPM_NAME | head -1`
    RPM_RELEASE=`rpm --qf "%{RELEASE}\n" -q $RPM_NAME | head -1`


    bash ${srcdir}/test_probes_rpmverifypackage.xml.sh \
        $RPM_NAME $RPM_EPOCH $RPM_ARCH $RPM_VERSION $RPM_RELEASE > $DF

    [ -f $RF ] && rm -f $RF
    $OSCAP oval eval --results $RF $DF

    result=$RF
    p="/oval_results/results/system/"
    q=$p"oval_system_characteristics/system_data/"
    assert_exists 1 '/oval_results/generator/oval:schema_version[text()="5.11"]'
    assert_exists 1 $p'tests/test'
    assert_exists 1 $p'oval_system_characteristics/system_data'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:name'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:name[text()="'$RPM_NAME'"]'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:epoch'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:epoch[text()="'$RPM_EPOCH'"]'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:version'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:version[text()="'$RPM_VERSION'"]'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:release'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:release[text()="'$RPM_RELEASE'"]'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:arch'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:arch[text()="'$RPM_ARCH'"]'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:extended_name'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:dependency_check_passed'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:dependency_check_passed[@datatype="boolean"]'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:verification_script_successful'
    assert_exists 1 $q'lin-sys:rpmverifypackage_item/lin-sys:verification_script_successful[@datatype="boolean"]'

    $OSCAP oval validate $RF

    rm $RF
    rm $DF
}

# Test Cases.

function test_probes_rpmverifypackage_epoch {
    RPM_package_1=$(rpm -qa --qf "%{NAME}\t%{EPOCH}\n" | grep -v "(none)" | sort -u | head -1 | cut -f 1) || ret=$?
    test_probes_rpmverifypackage $RPM_package_1
}

function test_probes_rpmverifypackage_noepoch {
    RPM_package_2=$(rpm -qa --qf "%{NAME}\t%{EPOCH}\n" | grep "(none)" | sort -u | head -1 | cut -f 1) || ret=$?
    test_probes_rpmverifypackage $RPM_package_2
}

# Testing.

test_init

test_run "test_probes_rpmverifypackage_epoch" test_probes_rpmverifypackage_epoch
test_run "test_probes_rpmverifypackage_noepoch" test_probes_rpmverifypackage_noepoch

test_exit
