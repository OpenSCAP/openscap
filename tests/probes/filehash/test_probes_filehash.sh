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

function test_probes_filehash {

    export OVAL_PROBE_DIR=`pwd`/../../../src/OVAL/probes/
    export LD_LIBRARY_PATH=`pwd`/../../../src/.libs
    export OSCAP_SCHEMA_PATH=$srcdir/../../../schemas

    if [ ! -x ${OVAL_PROBE_DIR}/probe_filehash ]; then		
	echo -e "Probe filehash does not exist!\n" 
	return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local DEFFILE="test_probes_filehash.xml"
    local RESFILE="results.xml"

    [ -f $RESFILE ] && rm -f $RESFILE

    eval "which md5sum  > /dev/null 2>&1"    
    if [ ! $? -eq 0 ]; then		
	echo -e "No md5sum found in $PATH!\n" 
	return 255; # Test is not applicable.
    fi

    eval "which sha1sum  > /dev/null 2>&1"    
    if [ ! $? -eq 0 ]; then		
	echo -e "No sha1sum found in $PATH!\n" 
	return 255; # Test is not applicable.
    fi

    bash ${srcdir}/test_probes_filehash.xml.sh > $DEFFILE
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

	COUNT=120; ID=1
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

test_init "test_probes_filehash.log"

test_run "test_probes_filehash" test_probes_filehash

test_exit
