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

function test_probes_runlevel_A {

    export OVAL_PROBE_DIR=`pwd`/../../../src/OVAL/probes/
    export LD_LIBRARY_PATH=`pwd`/../../../src/.libs
    export OSCAP_SCHEMA_PATH=$srcdir/../../../schemas

    if [ ! -x ${OVAL_PROBE_DIR}/probe_runlevel ]; then		
	echo -e "Probe runlevel does not exist!\n" 
	return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local DEFFILE="test_probes_runlevel_A.xml"
    local RESFILE="results_A.xml"
   
    [ -f $RESFILE ] && rm -f $RESFILE

    eval "which chkconfig > /dev/null 2>&1"    
    if [ ! $? -eq 0 ]; then	
	echo -e "No chkconfig found in path!\n" 
	return 255; # Test is not applicable.
    fi

    bash ${srcdir}/test_probes_runlevel_A.xml.sh > $DEFFILE
    ../../../utils/.libs/oscap oval eval --result-file $RESFILE $DEFFILE
        
    if [ -f $RESFILE ]; then

	for S in `chkconfig --list | awk '{print $1}'`; do
	    for L in `chkconfig $S --list | awk '{print $2 " " $3 " " $4 " " $5 " " $6 " " $7 " " $8}'`; do
		LEVEL=`echo $L | awk -F : '{print $1}'`
		STATE=`echo $L | awk -F : '{print $2}'`
	    
		for SUFFIX in "T F"; do
		    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${S}-${LEVEL}-${STATE}-${SUFFIX}"`
		    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${S}-${LEVEL}-${STATE}-${SUFFIX}\""`

		    if (echo $TEST_RES | grep -q "result=\"true\""); then
			RES="TRUE"
		    elif (echo $TEST_RES | grep -q "result=\"false\""); then
			RES="FALSE"
		    else
			RES="ERROR"
		    fi
		    
		    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
			CMT="TRUE"
		    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
			CMT="FALSE"
		    else
			CMT="ERROR"
		    fi
		    
		    if [ ! $RES = $CMT ]; then
			echo "Result of test:${S}-${LEVEL}-${STATE}-${SUFFIX} should be ${CMT}!" 
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

    export OVAL_PROBE_DIR=`pwd`/../../../src/OVAL/probes/
    export LD_LIBRARY_PATH=`pwd`/../../../src/.libs
    export OSCAP_SCHEMA_PATH=$srcdir/../../../schemas

    if [ ! -x ${OVAL_PROBE_DIR}/probe_runlevel ]; then		
	echo -e "Probe runlevel does not exist!\n" 
	return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local DEFFILE="test_probes_runlevel_B.xml"
    local RESFILE="results_B.xml"
    
    eval "which chkconfig > /dev/null 2>&1"    
    if [ ! $? -eq 0 ]; then	
	echo -e "No chkconfig found in $PATH!\n" 
	return 255; # Test is not applicable.
    fi

    local SERVICE_A=`chkconfig --list | grep "3:on" | head -1 | awk '{print $1}'`
    local SERVICE_B=`chkconfig --list | grep "3:off" | head -1 | awk '{print $1}'`

    bash ${srcdir}/test_probes_runlevel_B.xml.sh $SERVICE_A $SERVICE_B > $DEFFILE
    ../../../utils/.libs/oscap oval eval --result-file $RESFILE $DEFFILE
        
    if [ -f $RESFILE ]; then

	COUNT=13; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    DEF_DEF=`cat "$DEFFILE" | grep "id=\"oval:1:def:${ID}\""`
	    DEF_RES=`cat "$RESFILE" | grep "definition_id=\"oval:1:def:${ID}\""`

	    if (echo $DEF_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $DEF_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $DEF_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $DEF_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of oval:1:def:${ID} should be ${CMT}!" 
		ret_val=$[$ret_val + 1]
	    fi

	    ID=$[$ID+1]
	done

	COUNT=40; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${ID}\""`

	    if (echo $TEST_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $TEST_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of test:${ID} should be ${CMT}!" 
		ret_val=$[$ret_val + 1]
	    fi
	    
	    ID=$[$ID+1]
	done
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
