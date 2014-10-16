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

function test_probes_interface {
    require 'ipcalc' || return 255

    local ret_val=0;
    local DF="test_probes_interface.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    if [ ! -x test_probes_interface ]; then		
	echo -e "Testing binary not found!\n" 
	return 255; # Test is not applicable.
    fi

    bash ${srcdir}/test_probes_interface.xml.sh `pwd`/test_probes_interface \
	> $DF
    LINES=$?

    $OSCAP oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF $LINES
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_interface.log"

test_run "test_probes_interface" test_probes_interface

test_exit
