#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Testing Helpers.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

# Reporting results.
function report_result {    
    printf "+ %-32s" ${1}; echo -n "${1} " >&2
    if [ $2 -eq 0 ]; then 
	echo "[ PASS ]"; echo "PASS" >&2
    else 
	echo "[ FAIL ]"; echo "FAIL" >&2
    fi
}

# Compare content of two XML files.
function xml_cmp {
    local ret_val=0
    A=`mktemp` && sed "s/'/\"/g" $1 | sed 's|/>| />|g' | sed 's/^\s*//g' | sed 's/  / /g' > $A
    B=`mktemp` && sed "s/'/\"/g" $2 | sed 's|/>| />|g' | sed 's/^\s*//g' | sed 's/  / /g' > $B
    diff -wbB $A $B > diff.out 
    if [ $? -ne 0 ]; then
	for L in `cat diff.out | grep -v [\<\>-] | tr '\n' ' '`; do
	    L_A=`echo $L | awk -F 'c' '{print $1}'`
	    L_B=`echo $L | awk -F 'c' '{print $2}'`
	    A_1=`mktemp` && sed -n "${L_A}p" $A > $A_1
	    B_1=`mktemp` && sed -n "${L_B}p" $B > $B_1
	    diff -wbB $A_1 $B_1 1>&2
	    if [ ! $? -eq 0 ]; then
		cat $A_1 | tr ' ' '\n' | sort > $A_1
		cat $B_1 | tr ' ' '\n' | sort > $B_1
		diff -wbB $A_1 $B_1 1>&2 || ret_val=1
	    fi
	done
    fi	
    return $ret_val
}

