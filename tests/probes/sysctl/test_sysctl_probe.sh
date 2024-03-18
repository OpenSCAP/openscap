#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function perform_test {
	probecheck "sysctl" || return 255

	result=`mktemp`
	stderr=`mktemp`
	hostname=`hostname`
	case $(uname) in
		FreeBSD)
			$OSCAP oval eval --results $result $srcdir/test_sysctl_probe_freebsd.oval.xml 2>$stderr
			;;
		*)
			$OSCAP oval eval --results $result $srcdir/test_sysctl_probe.oval.xml 2>$stderr
			;;
	esac

	[ ! -s $stderr ]
	case $(uname) in
		FreeBSD)
			assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:name[text()='kern.hostname']"
			assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:value[text()='$hostname']"
			;;
		*)
			assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:name[text()='kernel.hostname']"
			assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:value[text()='$hostname']"
			;;
	esac

	rm $result
	rm $stderr
}

perform_test
