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

. ${srcdir}/runlevel_helper.sh

# Test Cases.

function test_probes_runlevel_A {

    probecheck "runlevel" || return 255
    require "chkconfig" || return 255
    require "egrep" || return 255
    require "awk" || return 255

    local ret_val=0;
    local DF="test_probes_runlevel_A.xml"
    local RF="results_A.xml"
   
    [ -f $RF ] && rm -f $RF

    bash ${srcdir}/test_probes_runlevel_A.xml.sh > $DF
    $OSCAP oval eval --results $RF $DF
        
    if [ -f $RF ]; then

	for S in `get_services_list`; do
	    for L in `get_service_runlevels ${S}`; do
		LEVEL=`echo $L | awk -F: '{print $1}'`
		STATE=`echo $L | awk -F: '{print $2}'`
	    
		for SUFFIX in T F; do
		    TEST_DEF=`grep "id=\"oval:${S}-${LEVEL}-${STATE}-${SUFFIX}:tst:1\"" $DF`
		    TEST_RES=`grep "test_id=\"oval:${S}-${LEVEL}-${STATE}-${SUFFIX}:tst:1\"" $RF`

		    if (echo $TEST_RES | grep "result=\"true\"" >/dev/null); then
			RES="TRUE"
		    elif (echo $TEST_RES | grep "result=\"false\"" >/dev/null); then
			RES="FALSE"
		    else
			RES="ERROR"
		    fi
		    
		    if (echo $TEST_DEF | grep "comment=\"true\"" >/dev/null); then
			CMT="TRUE"
		    elif (echo $TEST_DEF | grep "comment=\"false\"" >/dev/null); then
			CMT="FALSE"
		    else
			CMT="ERROR"
		    fi
		    
		    if [ ! $RES = $CMT ]; then
			echo "Result of oval:${S}-${LEVEL}-${STATE}-${SUFFIX}:tst:1 should be ${CMT}!" 
			ret_val=$[$ret_val + 1]
		    fi
		    
		done
	    done
	done
    else 
	ret_val=1
    fi

    return $ret_val
}

function test_probes_runlevel_B {

    probecheck "runlevel" || return 255

    export OVAL_PROBE_DIR=`pwd`/../../../src/OVAL/probes/
    export LD_LIBRARY_PATH=`pwd`/../../../src/.libs
    export OSCAP_SCHEMA_PATH=$srcdir/../../../schemas

    local ret_val=0;
    local DF="test_probes_runlevel_B.xml"
    local RF="results_B.xml"
    
    local SERVICE_A=`get_services_matching 3 on | head -1`
    local SERVICE_B=`get_services_matching 3 off | head -1`

    bash ${srcdir}/test_probes_runlevel_B.xml.sh $SERVICE_A $SERVICE_B > $DF
    $OSCAP oval eval --results $RF $DF
        
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 40
	ret_val=$?
    else 
	ret_val=1
    fi
    
    return $ret_val
}

# Testing.

test_init "test_probes_runlevel.log"

test_run "test_probes_runlevel_A" test_probes_runlevel_A
test_run "test_probes_runlevel_B" test_probes_runlevel_B

test_exit
