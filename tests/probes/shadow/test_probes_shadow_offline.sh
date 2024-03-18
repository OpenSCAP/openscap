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

set -e -o pipefail

# Test Cases.

function test_probes_shadow {

    probecheck "shadow" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_shadow_offline.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(make_temp_dir /tmp "test_offline_mode_shadow")
    mkdir -p "${tmpdir}/etc"
    echo "root:!locked::0:99999:7:::" > "${tmpdir}/etc/shadow"
    set_chroot_offline_test_mode "${tmpdir}"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode
    rm -rf "${tmpdir}"

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF $LINES
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init

test_run "test_probes_shadow" test_probes_shadow

test_exit
