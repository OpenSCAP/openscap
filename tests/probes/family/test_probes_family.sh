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

# Test cases.

function test_probes_family {

    probecheck "family" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_family.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 7 && verify_results "tst" $DF $RF 42
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init

test_run "test_probes_family" test_probes_family

test_exit
