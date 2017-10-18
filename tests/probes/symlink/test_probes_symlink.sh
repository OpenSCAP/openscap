#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_symlink {
    probecheck "symlink" || return 255

    DF="test_probes_symlink.xml"
    RF="results.xml"

    rm -f $RF

    tmpdir=$(mktemp -t -d "test_symlink.XXXXXX")

    touch $tmpdir/some_file
    touch $tmpdir/file_to_remove
    ln -s $tmpdir/some_file $tmpdir/normal_symlink
    ln -s $tmpdir/file_to_remove $tmpdir/broken_symlink
    rm -f $tmpdir/file_to_remove
    ln -s $tmpdir/circular_symlink $tmpdir/circular_symlink
    touch $tmpdir/other_file
    ln -s $tmpdir/other_file $tmpdir/middle_symlink
    ln -s $tmpdir/middle_symlink $tmpdir/chained_symlink

    bash ${srcdir}/test_probes_symlink.xml.sh $tmpdir > "$DF"

    $OSCAP oval eval --results $RF $DF

    result=$RF

    p='oval_results/results/system/oval_system_characteristics/'
    assert_exists 6 $p'collected_objects/object'
    assert_exists 2 $p'collected_objects/object[@flag="complete"]'
    assert_exists 2 $p'system_data/unix-sys:symlink_item'
    assert_exists 2 $p'system_data/unix-sys:symlink_item[@status="exists"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'$tmpdir/normal_symlink'"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="'$tmpdir'/some_file"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:filepath[text()="'$tmpdir/chained_symlink'"]'
    assert_exists 1 $p'system_data/unix-sys:symlink_item/unix-sys:canonical_path[text()="'$tmpdir'/other_file"]'
    assert_exists 2 $p'collected_objects/object[@flag="error"]'
    assert_exists 2 $p'collected_objects/object[@flag="does not exist"]'
    assert_exists 4 $p'collected_objects/object/message'

    rm -rf $tmpdir
    rm -f $RF
    rm -f $DF
}

test_probes_symlink
