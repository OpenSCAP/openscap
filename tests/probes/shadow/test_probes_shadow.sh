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

function test_probes_shadow {

    if [ ! -x ${OVAL_PROBE_DIR}/probe_shadow ]; then		
	echo -e "Probe shadow does not exist!\n" 
	return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local DEFFILE="test_probes_shadow.xml"
    local RESFILE="results.xml"

    [ -f $RESFILE ] && rm -f $RESFILE

    eval "cat /etc/shadow > /dev/null 2>&1"    
    if [ ! $? -eq 0 ]; then	
	echo -e "Can't read /etc/shadow!\n" 
	return 255; # Test is not applicable.
    fi
   
    bash ${srcdir}/test_probes_shadow.xml.sh > $DEFFILE
    ../../../utils/.libs/oscap oval eval --result-file $RESFILE $DEFFILE
        
    if [ -f $RESFILE ]; then

	DEF_DEF=`cat "$DEFFILE" | grep "id=\"oval:1:def:1\""`
	DEF_RES=`cat "$RESFILE" | grep "definition_id=\"oval:1:def:1\""`

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
	    echo "Result of definition:1 should be ${CMT}!" 
	    ret_val=$[$ret_val + 1]
	fi
	
	COUNT=`cat /etc/shadow | wc -l`; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TST_DEF=`cat "$DEFFILE" | grep "id=\"oval:1:tst:${ID}\""`
	    TST_RES=`cat "$RESFILE" | grep "test_id=\"oval:1:tst:${ID}\""`

	    if (echo $TST_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $TST_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $TST_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $TST_DEF | grep -q "comment=\"false\""); then
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

test_init "test_probes_shadow.log"

test_run "test_probes_shadow" test_probes_shadow

test_exit
