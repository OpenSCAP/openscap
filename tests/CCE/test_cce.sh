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

    cat >> test_cce_01.txt <<EOF
ID: CCE-3416-5
Description: The rhnsd service should be enabled or disabled as appropriate.
Reference Source: NSA Guide to the Secure Configuration of Red Hat Enterprise Linux 5 (Section 2.1.2.2)
Reference Value: disabled
Technical Mechanism: via chkconfig
Available Parameter Choices: enabled
Available Parameter Choices: disabled

EOF

    cat >> test_cce_02.txt <<EOF
ID: CCE-4218-4
Description: The rhnsd service should be enabled or disabled as appropriate.
Reference Source: NSA Guide to the Secure Configuration of Red Hat Enterprise Linux 5 (Section 2.1.2.2)
Reference Value: disabled
Technical Mechanism: via chkconfig
Available Parameter Choices: enabled
Available Parameter Choices: disabled

EOF

    cat test_cce_01.txt test_cce_02.txt > test_cce_all.txt
    ret_val=$?

    return $ret_val
}

# Test Cases.

# Creates a new CCE and frees it afterwards.
function test_cce_smoke {
    local ret_val=0;
    
    ./test_cce --smoke-test
    ret_val=$?
    
    return $ret_val
}

# Validation of valid CCE XML.
function test_cce_validate_valid_xml {
    local ret_val=0;

    ./test_cce --validate ${srcdir}/CCE/cce-sample.xml >&2
    ret_val=$?
    
    return $ret_val
}

# Validation of invalid CCE XML.
function test_cce_validate_invalid_xml {
    local ret_val=0;

    ./test_cce --validate ${srcdir}/CCE/cce-sample-invalid.xml >&2
    [ $? -eq 2 ] || ret_val=1
    
    return $ret_val
}

# Validation of damaged CCE XML.
function test_cce_validate_damaged_xml {
    local ret_val=0;

    ./test_cce --validate ${srcdir}/CCE/cce-sample-damaged.xml >&2
    [ $? -eq 2 ] || ret_val=1
    
    return $ret_val
}

# Parse imported CCE XML, print out content and check it.
function test_cce_parse_xml {
    local ret_val=0;

    ./test_cce --parse ${srcdir}/CCE/cce-sample.xml > \
	test_cce_tc05.out
    ret_val=$?

    cat test_cce_tc05.out >&2

    if [ $ret_val -eq 0 ]; then
	cmp test_cce_tc05.out test_cce_all.txt >&2
	ret_val=$?
    fi
    
    return $ret_val
}

# Import CCE XML and search for all CCE contained in it.
function test_cce_search_existing {
    local ret_val=0;

    ./test_cce --search ${srcdir}/CCE/cce-sample.xml "CCE-3416-5" > \
	test_cce_tc06_01.out
    ret_val=$[$ret_val + $?]
    cat test_cce_tc06_01.out >&2
    cmp test_cce_tc06_01.out test_cce_01.txt >&2
    ret_val=$[$ret_val + $?]

    ./test_cce --search ${srcdir}/CCE/cce-sample.xml "CCE-4218-4" > \
	test_cce_tc06_02.out
    ret_val=$[$ret_val + $?]
    cat test_cce_tc06_02.out >&2
    cmp test_cce_tc06_02.out test_cce_02.txt >&2
    ret_val=$[$ret_val + $?]

    return $ret_val
}

# Import CCE XML and search for non-existing CCE.
function test_cce_search_non_existing {
    local ret_val=0;

    ./test_cce --search ${srcdir}/CCE/cce-sample.xml "CCE-0000-0" > out
    ret_val=$?
    cat out >&2	
    if [ $ret_val -eq 0 ]; then
	grep "Not found!" out
	ret_va=$?
    fi

    return $ret_val
}


# Cleanup.
function test_cce_cleanup {     
    local ret_val=0;    
    
    rm  out \
        test_cce_01.txt \
	test_cce_02.txt \
	test_cce_all.txt \
	test_cce_tc05.out \
	test_cce_tc06_01.out \
	test_cce_tc06_02.out 

    ret_val=$?

    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_cce.log

exec 2>$log

test_cce_setup   
ret_val=$? 
report_result "test_cce_setup" $ret_val 
result=$[$result+$ret_val]

test_cce_smoke
ret_val=$? 
report_result "test_cce_smoke" $ret_val
result=$[$result+$ret_val]   

test_cce_validate_valid_xml
ret_val=$? 
report_result "test_cce_validate_valid_xml" $ret_val 
result=$[$result+$ret_val]   

test_cce_validate_invalid_xml
ret_val=$? 
report_result "test_cce_validate_invalid_xml" $ret_val 
result=$[$result+$ret_val]   

test_cce_validate_damaged_xml
ret_val=$? 
report_result "test_cce_validate_damaged_xml" $ret_val 
result=$[$result+$ret_val]   

test_cce_parse_xml
ret_val=$? 
report_result "test_cce_parse_xml" $ret_val 
result=$[$result+$ret_val]   

test_cce_search_existing
ret_val=$? 
report_result "test_cce_search_existing" $ret_val 
result=$[$result+$ret_val]   

test_cce_search_non_existing
ret_val=$? 
report_result "test_cce_search_non_existing" $ret_val 
result=$[$result+$ret_val]   

test_cce_cleanup 
ret_val=$?
report_result "test_cce_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "${log} (in tests dir)"

exit $result
