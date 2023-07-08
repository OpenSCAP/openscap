#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function perform_test {
	probecheck "sysctl" || return 255
	[ $(uname) == "Linux" ] || return 255

	result=`mktemp`
	stderr=`mktemp`
	hostname="fake.host.name.me"

	tmpdir=$(make_temp_dir /tmp "test_offline_mode_sysctl")
	mkdir -p "${tmpdir}/proc/sys/kernel"
	echo "${hostname}" > "${tmpdir}/proc/sys/kernel/hostname"
	set_chroot_offline_test_mode "${tmpdir}"

	$OSCAP oval eval --results $result $srcdir/test_sysctl_probe.oval.xml 2>$stderr

	unset_chroot_offline_test_mode
	rm -rf "${tmpdir}"

	[ ! -s $stderr ]
	assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:name[text()='kernel.hostname']"
	assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:value[text()='$hostname']"

	rm $result
	rm $stderr
}

perform_test
