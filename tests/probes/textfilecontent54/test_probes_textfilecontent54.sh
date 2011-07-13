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

function test_probes_textfilecontent54 {

    if [ ! -x ${OVAL_PROBE_DIR}/probe_textfilecontent54 ]; then		
	echo -e "Probe textfilecontent54 does not exist!\n" 
	return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local DEFFILE="${srcdir}/test_probes_textfilecontent54.xml"
    local RESFILE="results.xml"
    
    [ -f $RESFILE ] && rm -f $RESFILE

    local FILE_A="/tmp/test_probes_textfilecontent54.tmp_file"
    local FILE_B="/tmp/test_probes_textfilecontent54.tmp_file_empty"
    local FILE_C="/tmp/test_probes_textfilecontent54.tmp_file_non_existing"

    touch "$FILE_A"
    touch "$FILE_B"

    echo "valid_key = valid_value" > "$FILE_A"
    echo "valid_key = valid_value" >> "$FILE_A"

    ../../../utils/.libs/oscap oval eval --results $RESFILE $DEFFILE
    
    if [ -f $RESFILE ]; then

	COUNT=13; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    DEF_DEF=`cat "$DEFFILE" | grep "id=\"oval:0:def:${ID}\""`
	    DEF_RES=`cat "$RESFILE" | grep "definition_id=\"oval:0:def:${ID}\""`

	    if (echo $DEF_RES | grep "result=\"true\"" >/dev/null); then
		RES="TRUE"
	    elif (echo $DEF_RES | grep "result=\"false\"" >/dev/null); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $DEF_DEF | grep "comment=\"true\"" >/dev/null); then
		CMT="TRUE"
	    elif (echo $DEF_DEF | grep "comment=\"false\"" >/dev/null); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of oval:0:def:${ID} should be ${CMT}!" 
		ret_val=$[$ret_val + 1]
	    fi

	    ID=$[$ID+1]
	done

	COUNT=16; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"oval:0:tst:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"oval:0:tst:${ID}\""`

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
		echo "Result of oval:0:tst:${ID} should be ${CMT}!" 
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

test_init "test_probes_textfilecontent54.log"

test_run "test_probes_textfilencontent54" test_probes_textfilecontent54

test_exit
