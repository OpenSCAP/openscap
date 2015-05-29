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

    local def_id=$2
    local tst_id=$3
    local ret_val=1
    local DF="$srcdir/$1"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
        verify_results "def" $DF $RF $def_id && verify_results "tst" $DF $RF $tst_id
        ret_val=$?
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_file.log"

test_run "test_probes_file" test_probes_file test_probes_file.xml 13 204
test_run "test recurse_file_system=defined" test_probes_file test_recurse.xml 1 1

test_exit
