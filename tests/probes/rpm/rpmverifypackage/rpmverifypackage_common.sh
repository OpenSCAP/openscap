#!/usr/bin/env bash

# Copyright 2015-2019 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Probes Test Suite.
# Tests the rpmverifypackage probe.
#
# Authors:
#      Jan Černý <jcerny@redhat.com>
#      Evgenii Kolesnikov <ekolesni@redhat.com>

. $srcdir/../rpm_common.sh

function test_probes_rpmverifypackage {

    probecheck "rpmverifypackage" || return 255
    require "rpm" || return 255
    DF="test_probes_rpmverifypackage.xml"
    RF="${builddir}/tests/probes/rpm/rpmverifypackage/results.xml"

    RPM_NAME=$1
    RPM_EPOCH=`rpm_query $RPM_NAME EPOCH`
    RPM_ARCH=`rpm_query $RPM_NAME ARCH`
    RPM_VERSION=`rpm_query $RPM_NAME VERSION`
    RPM_RELEASE=`rpm_query $RPM_NAME RELEASE`

    bash ${srcdir}/rpmverifypackage.xml.sh \
        $RPM_NAME $RPM_EPOCH $RPM_ARCH $RPM_VERSION $RPM_RELEASE > $DF

    rm -f $RF

    $OSCAP oval eval --verbose INFO --results $RF $DF

    result=$RF

    $OSCAP oval validate $RF

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

    rm -f $RF
    rm -f $DF
}
