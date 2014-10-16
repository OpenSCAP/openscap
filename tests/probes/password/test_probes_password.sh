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

function test_probes_password {

    probecheck "password" || return 255

    local ret_val=0;
    local DF="test_probes_password.xml"
    local RF="results.xml"
   
    [ -f $RF ] && rm -f $RF

    eval "cat /etc/passwd > /dev/null 2>&1"    
    if [ ! $? -eq 0 ]; then	
	echo -e "Can't read /etc/passwd!\n" 
	return 255; # Test is not applicable.
    fi

    LINES=`cat /etc/passwd | wc -l`

    bash ${srcdir}/test_probes_password.xml.sh > $DF
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

test_init "test_probes_password.log"

test_run "test_probes_password" test_probes_password

test_exit
