#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CCE Module Test Suite.
#
# Created on: Dec 7, 2009
#
# Authors:
#      Ondrej Moris, <omoris@redhat.com>

. ${srcdir}/test_common.sh

# Setup.
function test_cce_setup {
    local ret_val=0

    cat >> ${srcdir}/test_cce_01.txt <<EOF
ID: CCE-3416-5
Description: The rhnsd service should be enabled or disabled as appropriate.
Reference Source: NSA Guide to the Secure Configuration of Red Hat Enterprise Linux 5 (Section 2.1.2.2)
Reference Value: disabled
Technical Mechanism: via chkconfig
Available Parameter Choices: enabled
Available Parameter Choices: disabled

EOF

    cat >> ${srcdir}/test_cce_02.txt <<EOF
ID: CCE-4218-4
Description: The rhnsd service should be enabled or disabled as appropriate.
Reference Source: NSA Guide to the Secure Configuration of Red Hat Enterprise Linux 5 (Section 2.1.2.2)
Reference Value: disabled
Technical Mechanism: via chkconfig
Available Parameter Choices: enabled
Available Parameter Choices: disabled

EOF

    cat ${srcdir}/test_cce_01.txt ${srcdir}/test_cce_02.txt > ${srcdir}/test_cce_all.txt
    ret_val=$?

    return $ret_val
}

# Test Cases.

function test_cce_tc01 {
    local ret_val=0;

    ${srcdir}/test_cce --smoke-test
    ret_val=$?
    
    return $ret_val
}

function test_cce_tc02 {
    local ret_val=0;

    ${srcdir}/test_cce --validate ${srcdir}/CCE/cce-sample.xml >&2
    ret_val=$?
    
    return $ret_val
}

function test_cce_tc03 {
    local ret_val=0;

    ${srcdir}/test_cce --validate ${srcdir}/CCE/cce-sample-invalid.xml >&2
    [ $? -eq 2 ] || ret_val=1
    
    return $ret_val
}

function test_cce_tc04 {
    local ret_val=0;

    ${srcdir}/test_cce --validate ${srcdir}/CCE/cce-sample-damaged.xml >&2
    [ $? -eq 2 ] || ret_val=1
    
    return $ret_val
}


function test_cce_tc05 {
    local ret_val=0;

    ${srcdir}/test_cce --parse ${srcdir}/CCE/cce-sample.xml > ${srcdir}/test_cce_tc05.out
    ret_val=$?

    cat test_cce_tc05.out >&2

    if [ $ret_val -eq 0 ]; then
	cmp ${srcdir}/test_cce_tc05.out ${srcdir}/test_cce_all.txt >&2
	ret_val=$?
    fi
    
    return $ret_val
}

function test_cce_tc06 {
    local ret_val=0;

    ${srcdir}/test_cce --search ${srcdir}/CCE/cce-sample.xml "CCE-3416-5" > ${srcdir}/test_cce_tc06_01.out
    ret_val=$[$ret_val + $?]
    cat ${srcdir}/test_cce_tc06_01.out >&2
    cmp ${srcdir}/test_cce_tc06_01.out ${srcdir}/test_cce_01.txt >&2
    ret_val=$[$ret_val + $?]

    ${srcdir}/test_cce --search ${srcdir}/CCE/cce-sample.xml "CCE-4218-4" > ${srcdir}/test_cce_tc06_02.out
    ret_val=$[$ret_val + $?]
    cat ${srcdir}/test_cce_tc06_02.out >&2
    cmp ${srcdir}/test_cce_tc06_02.out ${srcdir}/test_cce_02.txt >&2
    ret_val=$[$ret_val + $?]

    return $ret_val
}

function test_cce_tc07 {
    local ret_val=0;

    ${srcdir}/test_cce --search ${srcdir}/CCE/cce-sample.xml "CCE-0000-0" >&2
    [ ! $? -eq 1 ] && ret_val=1
    
    return $ret_val
}


# Cleanup.
function test_cce_cleanup {     
    local ret_val=0;    
    
    rm  ${srcdir}/test_cce_01.txt \
	${srcdir}/test_cce_02.txt \
	${srcdir}/test_cce_all.txt \
	${srcdir}/test_cce_tc05.out \
	${srcdir}/test_cce_tc06_01.out \
	${srcdir}/test_cce_tc06_02.out 

    ret_val=$?

    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=${srcdir}/test_cce.log

exec 2>$log

test_cce_setup   
ret_val=$? 
report_result "test_cce_setup" $ret_val
result=$[$result+$ret_val]

test_cce_tc01
ret_val=$? 
report_result "test_cce_tc01" $ret_val 
result=$[$result+$ret_val]   

test_cce_tc02
ret_val=$? 
report_result "test_cce_tc02" $ret_val 
result=$[$result+$ret_val]   

test_cce_tc03
ret_val=$? 
report_result "test_cce_tc03" $ret_val 
result=$[$result+$ret_val]   

test_cce_tc04
ret_val=$? 
report_result "test_cce_tc04" $ret_val 
result=$[$result+$ret_val]   

test_cce_tc05
ret_val=$? 
report_result "test_cce_tc05" $ret_val 
result=$[$result+$ret_val]   

test_cce_tc06
ret_val=$? 
report_result "test_cce_tc06" $ret_val 
result=$[$result+$ret_val]   

test_cce_cleanup 
ret_val=$?
report_result "test_cce_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "------------------------------------------"
echo "${log} (in tests directory)"

exit $result
