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
#      Evgenii Kolesnikov, <ekolesni@redhat.com>

. $builddir/tests/test_common.sh

# Test Cases.

function test_probes_password {

    probecheck "password" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_password_offline.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_password.XXXXXX")
    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode
    rm -rf $tempdir

    if [ -f $RF ]; then
        result=$RF
        assert_exists 1 'oval_results/results/system/tests/test[@test_id="oval:1:tst:1"][@result="not applicable"]'
        ret_val=$?
    else
        ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init

test_run "test_probes_password_offline" test_probes_password

test_exit
