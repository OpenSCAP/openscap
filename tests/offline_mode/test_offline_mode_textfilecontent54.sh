#!/bin/bash

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

    # prepare /bar.txt
    echo "Hello" > "$temp_dir/bar.txt"

    # prepare /zzz/foo.txt
    mkdir -p "$temp_dir/zzz"
    echo "Bye" > "$temp_dir/zzz/foo.txt"

    result="$(mktemp)"

    export OSCAP_PROBE_ROOT
    OSCAP_PROBE_ROOT="$temp_dir"
    $OSCAP oval eval --results $result $srcdir/textfilecontent54.oval.xml

    [ -s "$result" ]

    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data'

    tfc_item='/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item'
    assert_exists 2 $tfc_item

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/bar.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:path[text()="/"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="bar.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="Hello"]'

    assert_exists 1 $tfc_item'/ind-sys:filepath[text()="/zzz/foo.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:path[text()="/zzz"]'
    assert_exists 1 $tfc_item'/ind-sys:filename[text()="foo.txt"]'
    assert_exists 1 $tfc_item'/ind-sys:text[text()="Bye"]'

    rm -rf "$temp_dir"
    rm -f "$result"
}

# Testing.

test_init "test_offline_mode_textfilecontent54.log"

test_run "test_offline_mode_textfilecontent54" test_offline_mode_textfilecontent54

test_exit
