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

function test_probes_rpminfo {

    probecheck "rpminfo" || return 255
    require "rpm" || return 255

    local ret_val=0;
    local DF="test_probes_rpminfo.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    local RPM_A_NAME=`rpm --qf "%{NAME}\n" -qa | sort | uniq -u | sed -n '1p'`
    local RPM_B_NAME=`rpm --qf "%{NAME}\n" -qa | sort | uniq -u | sed -n '2p'`
    
    bash ${srcdir}/test_probes_rpminfo.xml.sh $RPM_A_NAME $RPM_B_NAME > $DF
    $OSCAP oval eval --results $RF $DF
        
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 200
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_rpminfo.log"

test_run "test_probes_rpminfo" test_probes_rpminfo

test_exit
