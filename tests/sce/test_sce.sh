#!/usr/bin/env bash

# Author:
#	Martin Preisler <mpreisle@redhat.com>

. ${srcdir}/../test_common.sh

# Test Cases.

function test_sce {

    local ret_val=0;
    local DEFFILE=${srcdir}/$1
    local RESFILE=$1.results

    [ -f $RESFILE ] && rm -f $RESFILE
    
    # FIXME: skip-valid is a temporary workaround, <target> tag isn't exported when using SCE
    ../../utils/.libs/oscap xccdf eval --results "$RESFILE" --profile "default" --skip-valid "$DEFFILE"
    
    # catch error from oscap tool
    ret_val=$?
    if [ $ret_val -eq 1 ]; then
	return 1
    fi

    LINES=`grep \<result\> "$RESFILE"`
    # catch error from grep
    ret_val=$?
    if [ $ret_val -eq 2 ]; then
	return 1
    fi

    # calculate return code
    echo "$LINES" | grep -q -v "pass"
    ret_val=$?
    if [ $ret_val -eq 1 ]; then
        return 0;
    elif [ $ret_val -eq 0 ]; then
	return 1;
    else
	return "$ret_val"
    fi
}

# Testing.
test_init "test_sce.log"

test_run "sce" test_sce sce_xccdf.xml 

test_exit

