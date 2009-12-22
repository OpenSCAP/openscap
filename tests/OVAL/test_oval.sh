#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap OVAL Module Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      Peter Vrabec, <pvrabec@redhat.com>
#      David Niemoller
#      Ondrej Moris, <omoris@redhat.com>

. ${srcdir}/test_common.sh

# Setup.
function test_oval_setup {
    local ret_val=0
    return $ret_val
}

# Test Cases.

function test_oval_definition {
    local ret_val=0;

    ./test_oval "${srcdir}/OVAL/scap-rhel5-oval.xml" \
	> test_oval_tc01.out
    ret_val=$?
    
    return $ret_val
}

function test_oval_syschar {
    local ret_val=0;

    ./test_syschar "${srcdir}/OVAL/composed-oval.xml" \
	           "${srcdir}/OVAL/system-characteristics.xml" \
	           > test_oval_tc02.out
    ret_val=$?
    
    return $ret_val
}

function test_oval_results {
    local ret_val=0;

    ./test_results "${srcdir}/OVAL/scap-rhel5-oval.xml" \
	                   "${srcdir}/OVAL/results.xml" \
	                   > test_oval_tc03.out
    ret_val=$?
    
    return $ret_val
}

# Cleanup.
function test_oval_cleanup {     
    local ret_val=0;    

    rm -f test_oval_tc01.out \
	  test_oval_tc02.out \
	  test_oval_tc03.out

    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_oval.log

exec 2>$log

test_oval_setup   
ret_val=$? 
report_result "test_oval_setup" $ret_val
result=$[$result+$ret_val]

test_oval_definition
ret_val=$? 
report_result "test_oval_definition" $ret_val 
result=$[$result+$ret_val]   

test_oval_syschar
ret_val=$? 
report_result "test_oval_syschar" $ret_val 
result=$[$result+$ret_val]   

test_oval_results
ret_val=$? 
report_result "test_oval_results" $ret_val 
result=$[$result+$ret_val]   

test_oval_cleanup 
ret_val=$?
report_result "test_oval_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result

