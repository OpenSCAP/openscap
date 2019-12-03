#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_offline_mode_symlink {
    probecheck "symlink" || return 255

    DF="test_offline_mode_symlink.xml"
    RF="offline_mode_results.xml"

    rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_symlink.XXXXXX")
    touch $tmpdir/some_file
    touch $tmpdir/file_to_remove
    ln -s /some_file $tmpdir/normal_symlink
    ln -s some_file $tmpdir/normal_symlink_relative
    ln -s /file_to_remove $tmpdir/broken_symlink
    ln -s file_to_remove $tmpdir/broken_symlink_relative
    rm -f $tmpdir/file_to_remove
    ln -s /circular_symlink $tmpdir/circular_symlink
    touch $tmpdir/other_file
    ln -s /other_file $tmpdir/middle_symlink
    ln -s /middle_symlink $tmpdir/chained_symlink
    mkdir $tmpdir/etc
    touch $tmpdir/etc/shadow
    ln -s /etc/shadow $tmpdir/etc/passwd
    touch $tmpdir/file_in_root
    ln -s ../../../../../file_in_root $tmpdir/path_traversal_symlink
    ln -s ./some_file $tmpdir/symlink_with_dot_in_path
#temporary ugly solution trying to ensure that files don't exist before being created
    rm -f /tmp/symlinktarget
    touch /tmp/symlinktarget
    rm -f /tmp/symlinktest
    ln -s /tmp/symlinktarget /tmp/symlinktest



    bash ${srcdir}/test_offline_mode_symlink.xml.sh "" > "$DF"
    export OSCAP_PROBE_ROOT="$tmpdir"
    $OSCAP oval eval --results $RF $DF

    result=$RF

    rm -f $DF
    rm -f /tmp/symlinktest
    rm -f /tmp/symlinktarget
    rm -rf $tmpdir

    p='oval_results/results/system/oval_system_characteristics/'
    assert_exists 12 $p'collected_objects/object'
    assert_exists 6 $p'collected_objects/object[@flag="complete"]'
    assert_exists 6 $p'system_data/unix-sys:symlink_item'
    assert_exists 6 $p'system_data/unix-sys:symlink_item[@status="exists"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'/normal_symlink'"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'/normal_symlink_relative'"]'
    assert_exists 3 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="/some_file"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'/chained_symlink'"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="/other_file"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'/etc/passwd'"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="/etc/shadow"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'/path_traversal_symlink'"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="/file_in_root"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'/symlink_with_dot_in_path'"]'
    assert_exists 3 $p'collected_objects/object[@flag="error"]'
    assert_exists 3 $p'collected_objects/object[@flag="does not exist"]'
    assert_exists 6 $p'collected_objects/object/message'

    rm -f $RF

}

test_init "test_offline_mode_symlink.log"

test_run "test_offline_mode_symlink" test_offline_mode_symlink

test_exit

