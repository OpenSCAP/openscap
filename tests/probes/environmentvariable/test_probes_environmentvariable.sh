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

function test_probes_environmentvariable {

    local ret_val=0;
    local DEFFILE="$1.xml"
    local RESFILE="$1.results.xml"

    [ -f $RESFILE ] && rm -f $RESFILE

    bash ${srcdir}/$1.xml.sh > $DEFFILE
    LINES=$?

    ../../../utils/.libs/oscap oval eval --results $RESFILE $DEFFILE
    
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
	    echo "Result of oval:1:def:1 should be ${CMT}!" 
	    ret_val=$[$ret_val + 1]
	fi
	
	COUNT=$LINES; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"oval:1:tst:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"oval:1:tst:${ID}\""`

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
		echo "Result of oval:1:tst:${ID} should be ${CMT}!" 
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

test_init "test_probes_environmentvariable.log"

test_run "test_probes_environmentvariable" test_probes_environmentvariable test_probes_environmentvariable
test_run "test_probes_environmentvariable" test_probes_environmentvariable test_probes_environmentvariable-fail

test_exit
