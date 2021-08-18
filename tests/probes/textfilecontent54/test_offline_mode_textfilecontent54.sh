#!/usr/bin/env bash

# Copyright 2018 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Test Suite
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_offline_mode_textfilecontent54 {
    temp_dir="$(mktemp -d)"

    mkdir -p $temp_dir/tmp/zzz
    mkdir -p /tmp/zzz

    # prepare /bar.txt
    echo "Hello from the inside" > "$temp_dir/tmp/bar.txt"
    echo "Hello from the outside" > "/tmp/bar.txt"

    # prepare /zzz/foo.txt
    mkdir -p "$temp_dir/tmp/zzz"
    echo "Bye from the inside" > "$temp_dir/tmp/zzz/foo.txt"
    echo "Bye from the outside" > "/tmp/zzz/foo.txt"

    # prepare file that is available only outside
    echo "I'm outside" > "/tmp/only_outside.txt"

    # prepare pattern matching files
    echo "bar filepath pattern" > "$temp_dir/tmp/bar.filepath"
    echo "foo filepath pattern" > "$temp_dir/tmp/zzz/foo.filepath"

    echo "bar filename pattern" > "$temp_dir/tmp/bar.filename"
    echo "foo filename pattern" > "$temp_dir/tmp/zzz/foo.filename"
    result="$(mktemp)"

    set_chroot_offline_test_mode "$temp_dir"
    $OSCAP oval eval --results $result $srcdir/test_offline_mode_textfilecontent54.xml
    unset_chroot_offline_test_mode

    [ -s "$result" ]

    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data'

    tfc_item='/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item'
    assert_exists 6 $tfc_item
    assert_exists 3 $tfc_item'/ind-sys:path[text()="/tmp"]'
    assert_exists 3 $tfc_item'/ind-sys:path[text()="/tmp/zzz"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/tmp/bar.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="bar.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="Hello from the inside"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/tmp/zzz/foo.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="foo.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="Bye from the inside"]'

    assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3" and @result="true"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/tmp/bar.filepath"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="bar.filepath"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="bar filepath pattern"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/tmp/zzz/foo.filepath"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="foo.filepath"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="foo filepath pattern"]'

    assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4" and @result="true"]'
    assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:5" and @result="true"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/tmp/bar.filename"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="bar.filename"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="bar filename pattern"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/tmp/zzz/foo.filename"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="foo.filename"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="foo filename pattern"]'

    assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:6" and @result="true"]'
    assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:7" and @result="true"]'

    rm -rf "$temp_dir"
    rm -f "$result"
    rm -f "/tmp/bar.txt" "/tmp/only_outside.txt"
    rm -rf "/tmp/zzz"
}

# Testing.

test_init "test_offline_mode_textfilecontent54.log"

test_run "test_offline_mode_textfilecontent54" test_offline_mode_textfilecontent54

test_exit
