#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>


. ../test_common.sh

# Test Cases.

function test_mitre {

    if [ ! -d /tmp/ValidationSupportFiles ]; then
        eval "which unzip > /dev/null 2>&1"    
        if [ ! $? -eq 0 ]; then	
            echo -e "No unzip found in $PATH!\n" 
	    return 255; # Test is not applicable.
        fi
        /usr/bin/unzip -u ValidationSupportFiles.zip -d /tmp
    fi

    local ret_val=0;
    local DEFFILE=$1
    local RESFILE="$DEFFILE".results

    [ -f $RESFILE ] && rm -f $RESFILE
    ../../utils/.libs/oscap oval eval --results $RESFILE $DEFFILE
    ret_val=$?
    
    if [ $ret_val -eq 0 ]; then
        return 0;
    elif [ $ret_val -eq 2 ]; then
	return 1;
    else
	return ret_val=$?
    fi
}

# Testing.
test_init "test_mitre.log"
test_run "ind-def_family_test.xml" test_mitre ind-def_family_test.xml
test_exit

