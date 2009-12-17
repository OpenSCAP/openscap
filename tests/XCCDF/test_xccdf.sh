#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap XCCDF Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#

. ${srcdir}/test_common.sh

# Setup.
function test_xccdf_setup {
    local ret_val=0    
    return $ret_val
}

# Test Cases.

function test_xccdf_import {
    local ret_val=0

    ./xccdf_dump "${srcdir}/XCCDF/scap-rhel5-xccdf.xml" > test_xccdf.out 2>&1
    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_xccdf_cleanup {
    local ret_val=0

    rm test_xccdf.out
    ret_val=$?
    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_xccdf.log

exec 2>$log

test_xccdf_setup
ret_val=$? 
report_result "test_xccdf_setup" $ret_val
result=$[$result+$ret_val]

test_xccdf_import
ret_val=$? 
report_result "test_xccdf_import" $ret_val
result=$[$result+$ret_val]

test_xccdf_cleanup
ret_val=$? 
report_result "test_xccdf_cleanup" $ret_val
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result
