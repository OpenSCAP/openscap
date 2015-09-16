#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

. ../../test_common.sh

set -e -o pipefail

function test_probes_rpmverifyfile {
    probecheck "rpmverifyfile" || return 255

    DF="$srcdir/test_probes_rpmverifyfile.xml"
    RF="results.xml"

    rm -f $RF

    $OSCAP oval eval --results $RF $DF

    result=$RF

    assert_exists 1 'oval_results/oval_definitions/tests/lin-def:rpmverifyfile_test'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nolinkto="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nosize="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nouser="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nogroup="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nomtime="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nomode="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nordev="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@noconfigfiles="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@noghostfiles="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nofiledigest="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:behaviors[@nocaps="true"]'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:name'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:epoch'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:version'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:release'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:arch'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:filepath'
    assert_exists 1 'oval_results/oval_definitions/objects/lin-def:rpmverifyfile_object/lin-def:filepath[text()="/etc/redhat-release"]'
    sc='oval_results/results/system/oval_system_characteristics/'
    sd=$sc'system_data/'
    assert_exists 1 $sc'collected_objects/object'
    assert_exists 1 $sc'collected_objects/object[@flag="complete"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item[@status="exists"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:name'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:epoch'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:version'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:release'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:arch'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:filepath'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:extended_name'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:size_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:size_differs[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:mode_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:mode_differs[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:filedigest_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:filedigest_differs[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:device_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:device_differs[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:link_mismatch'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:link_mismatch[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:ownership_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:ownership_differs[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:group_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:mtime_differs'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:group_differs[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:capabilities_differ'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:capabilities_differ[text()="not performed"]'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:configuration_file'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:documentation_file'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:ghost_file'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:license_file'
    assert_exists 1 $sd'lin-sys:rpmverifyfile_item/lin-sys:readme_file'

    rm -f $RF
}

test_probes_rpmverifyfile
