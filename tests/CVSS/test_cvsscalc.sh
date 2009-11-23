#!/bin/sh

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CVSS Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#

. ${srcdir}/test_common.sh

# Setup.
function test_cvss_setup {
    local ret_val=0
    
    return $ret_val
}

# Test Cases.

function test_cvss_tc00 {
    local ret_val=0

    ./test_cvsscalc 2>&1 > test_cvss.out
    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_cvss_cleanup {
    local ret_val=0

    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=test_cvss.log

exec 2>$log

#---- function ------#-------------------------- reporting -----------------------#--------------------------#
test_cvss_setup    ; ret_val=$? ; report_result "test_CVSS_setup"   $ret_val  ; result=$[$result+$ret_val]
test_cvss_tc00     ; ret_val=$? ; report_result 'Test_CVSS_TC01'    $ret_val  ; result=$[$result+$ret_val]   
test_cvss_cleanup  ; ret_val=$? ; report_result "test_CVSS_cleanup" $ret_val  ; result=$[$result+$ret_val]

echo "------------------------------------------"
echo "See ${srcdir}/${log}"

exit $result

