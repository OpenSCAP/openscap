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

. ../../test_common.sh

# Test Cases.

function test_probes_uname {
    
    probecheck "uname" || return 255
    require "uname" || return 255

    local ret_val=0;
    local DF="test_probes_uname.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF
    
    bash ${srcdir}/test_probes_uname.xml.sh > "$DF"
    $OSCAP oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 96
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_uname.log"

test_run "test_probes_uname" test_probes_uname

test_exit
