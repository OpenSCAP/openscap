#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      Peter Vrabec, <pvrabec@redhat.com>
#      David Niemoller
#      Ondrej Moris, <omoris@redhat.com>

. $builddir/tests/test_common.sh

# Test Cases.

function test_probes_filehash58 {

    probecheck "filehash58" || return 255
    require "md5sum" || return 255
    require "sha1sum" || return 255

    local ret_val=0;
    local DF="test_probes_filehash58.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    bash ${srcdir}/test_probes_filehash58.xml.sh > $DF
    $OSCAP oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 120
	ret_val=$?
    else 
	ret_val=1
    fi

    # The file was created as a side-effect of test_probes_filehash58.xml.sh
    [ $ret_val -eq 0 ] && rm -f /tmp/test_probes_filehash58.tmp

    return $ret_val
}


# $1: The chroot directory
function test_probes_filehash58_chroot {

    probecheck "filehash58" || return 255

    local ret_val=0;
    local DF="$srcdir/check_filehash_simple.xml"

    absolute_probe_root=$(cd "$1" && pwd)

    # oscap-chroot is not readily available during test run, so we use oscap + env var setting.
    result_keyword=$(OSCAP_PROBE_ROOT="$absolute_probe_root" $OSCAP oval eval "$DF" | grep oval_test_has_hash | grep -o '\w*$')
    
    [ "$result_keyword" == "$2" ] && return 0
    # vvv This is more a test error than a failure or "warning" vvv
    [ "$result_keyword" == "" ] && return 2
    return 1
}


function test_probes_filehash58_chroot_pass {
	local ret_val=0

	mkdir -p pass
	echo foo > pass/oval-test

	test_probes_filehash58_chroot pass true
	ret_val=$?
	rm -rf pass

	return $ret_val
}


function test_probes_filehash58_chroot_fail {
	local ret_val=0

	mkdir -p fail
	echo bar > fail/oval-test

	test_probes_filehash58_chroot fail false
	ret_val=$?
	rm -rf fail

	return $ret_val
}

# Testing.

test_init

test_run "test_probes_filehash58" test_probes_filehash58

test_run "test_probes_filehash58_chroot_fail" test_probes_filehash58_chroot_fail

test_run "test_probes_filehash58_chroot_pass" test_probes_filehash58_chroot_pass

test_exit
