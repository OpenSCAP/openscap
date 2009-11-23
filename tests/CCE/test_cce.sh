#!/bin/sh

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CCE Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#

. ${srcdir}/test_common.sh

# Setup.
function test_cce_setup {
    local ret_val=0
    
    return $ret_val
}

# Test Cases.

function test_cce_tc00 {
    local ret_val=0

    ./test_cce "${srcdir}/CCE/CCE_Sample.xml" CCE-3416-5 > test_cce.out.1
    ret_val=$?

    return $ret_val
}

function test_cce_tc01 {

    local ret_val=0

    ./test_cce "${srcdir}/CCE/CCE_Sample.xml" CCE-4218-4 > test_cce.out.2
    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_cce_cleanup {
    local ret_val=0

    rm test_cce.out.1 test_cce.out.2
    ret_val=$?
    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=test_cce.log

exec 2>$log

#---- function ------#-------------------------- reporting -----------------------#--------------------------#
test_cce_setup    ; ret_val=$? ; report_result "test_CCE_setup"   $ret_val  ; result=$[$result+$ret_val]
test_cce_tc00     ; ret_val=$? ; report_result 'Test_CCE_TC00'    $ret_val  ; result=$[$result+$ret_val]   
test_cce_tc01     ; ret_val=$? ; report_result 'Test_CCE_TC01'    $ret_val  ; result=$[$result+$ret_val]   
test_cce_cleanup  ; ret_val=$? ; report_result "test_CCE_cleanup" $ret_val  ; result=$[$result+$ret_val]

echo "------------------------------------------"
echo "See ${srcdir}/${log}"

exit $result
