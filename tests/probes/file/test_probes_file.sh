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

. ${srcdir}/../../test_common.sh

# Test Cases.

function test_probes_file {

    probecheck "file" || return 255

    local ret_val=0;
    local DF="$srcdir/test_probes_file.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 204
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

function test_recurse {

    probecheck "file" || return 255

    local ret_val=0
    local DF="$srcdir/test_recurse.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_file.log"

test_run "test_probes_file" test_probes_file
test_run "test recurse_file_system=defined" test_recurse

test_exit
