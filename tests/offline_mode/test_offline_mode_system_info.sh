#!/bin/bash

# Copyright 2018 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Test Suite
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

. ../test_common.sh

set -e -o pipefail

function test_offline_mode_system_info {
    temp_dir="$(mktemp -d)"

    # create a mock host name
    mkdir -p "$temp_dir/etc"
    echo "guest_system_host_name" > "$temp_dir/etc/hostname"

    # inject a mock grubenv file
    mkdir -p "$temp_dir/boot/grub2"
    cp "$srcdir/grubenv" "$temp_dir/boot/grub2/grubenv"

    result="$(mktemp)"

    export OSCAP_PROBE_ROOT
    OSCAP_PROBE_ROOT="$temp_dir"
    $OSCAP oval eval --results $result $srcdir/textfilecontent54.oval.xml

    [ -s "$result" ]

    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info'
    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/os_name[text()="GuestSystem"]'
    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/os_version[text()="(1.2.3-400.el7.x86_64)"]'
    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/architecture[text()="x86_64"]'
    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/primary_host_name[text()="guest_system_host_name"]'

    assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/interfaces'
    # Getting network interfaces information from the guest is not implemented,
    # check if there are no interfaces from the host
    assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_info/interfaces/interface'

    rm -rf "$temp_dir"
    rm -f "$result"
}

# Testing.

test_init "test_offline_mode_system_info.log"

test_run "test_offline_mode_system_info" test_offline_mode_system_info

test_exit
