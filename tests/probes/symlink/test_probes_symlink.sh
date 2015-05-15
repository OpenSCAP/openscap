#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

. ../../test_common.sh

set -e -o pipefail

function test_probes_symlink {
    probecheck "symlink" || return 255

    DF="${srcdir}/test_probes_symlink.xml"
    RF="results.xml"

    rm -f $RF
    rm -f some_file
    rm -f file_to_remove
    rm -f normal_symlink
    rm -f broken_symlink
    rm -f circular_symlink

    touch some_file
    touch file_to_remove
    ln -s some_file normal_symlink
    ln -s file_to_remove broken_symlink
    rm -f file_to_remove
    ln -s circular_symlink circular_symlink

    $OSCAP oval eval --results $RF $DF

    result=$RF

    p='oval_results/results/system/oval_system_characteristics/'
    assert_exists 5 $p'collected_objects/object'
    assert_exists 1 $p'collected_objects/object[@flag="complete"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item'
    assert_exists 1 $p'system_data/unix-sys:symlink_item[@status="exists"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="normal_symlink"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="some_file"]'
    assert_exists 2 $p'collected_objects/object[@flag="error"]'
    assert_exists 2 $p'collected_objects/object[@flag="does not exist"]'
    assert_exists 4 $p'collected_objects/object/message'

    rm -f some_file
    rm -f normal_symlink
    rm -f broken_symlink
    rm -f circular_symlink
    rm -f $RF
}

test_probes_symlink
