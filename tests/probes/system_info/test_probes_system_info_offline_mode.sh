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

function test_offline_mode_system_info_offline {
	os_name="Offline Linux"
	os_version="1.0 (Offline)"
	host_name="offline.example.com"

	# A root of its own, as a probe root is: its /etc/os-release links to
	# /usr/lib/os-release, as on most systems, and the link resolves in the
	# root.
	temp_dir="$(mktemp -d)"
	mkdir -p "$temp_dir/etc" "$temp_dir/usr/lib"
	printf 'NAME="%s"\nVERSION="%s"\n' "$os_name" "$os_version" > "$temp_dir/usr/lib/os-release"
	ln -s /usr/lib/os-release "$temp_dir/etc/os-release"
	echo "$host_name" > "$temp_dir/etc/hostname"
	echo "hostname=\"$host_name\"" > "$temp_dir/etc/rc.conf"

	result="$(mktemp)"

	set_chroot_offline_test_mode "$temp_dir"

	$OSCAP oval eval --results $result $srcdir/test_probes_system_info.oval.xml

	unset_chroot_offline_test_mode

	[ -s "$result" ]

	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info'
	assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_info/os_name[text()=\"${os_name}\"]"
	assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_info/os_version[text()=\"${os_version}\"]"
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/architecture[text()="Unknown"]'
	assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_info/primary_host_name[text()=\"${host_name}\"]"

	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info/interfaces'
	# Getting network interfaces information from the guest is not implemented,
	# check if there are no interfaces from the host
	assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_info/interfaces/interface'

	rm -rf "$temp_dir"
	rm -f "$result"
}

test_run "test_offline_mode_system_info_offline" test_offline_mode_system_info_offline
