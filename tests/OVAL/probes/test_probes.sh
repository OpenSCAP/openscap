#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

. ${srcdir}/test_common.sh

# Setup.
function test_probes_setup {
    local ret_val=0
    export OVAL_PROBE_DIR="`pwd`/../src/OVAL/probes/"
    return $ret_val
}

# Test Cases.

function test_probes_import {
    local ret_val=0;

    FILES=(
	"probes/rpminfo.xml"     \
	"probes/runlevel.xml"    \
	"probes/filecontent.xml" \
	"probes/file.xml"        \
	"probes/file-set.xml"    \
	"probes/file-set2.xml"   \
	"probes/file-set3.xml"
    )

    for F in "${FILES[@]}"; do
	./test_probes "${srcdir}/OVAL/${F}" > test_probes_tc01.out
	ret_val=$[$ret_val+$?]
    done

    return $ret_val
}

# Check if selected system characteristics were populated correctly. 
function test_probes_get_system_chars {
    local ret_val=0
   
    ./test_sysinfo 2>&1 > test_probes_tc02.out
    ret_val=$?
        
    [ `du test_probes_tc02.out | awk '{print $1}'` -eq 0 ] && ret_val=1
    
    if [ $ret_val -eq 0 ]; then
	grep -q "os_name: `uname`" test_probes_tc02.out || ret_val=1
	grep -q "os_version: `uname -v`" test_probes_tc02.out || ret_val=1
	grep -q "os_architecture: `uname -m`" test_probes_tc02.out || ret_val=1
	grep -q "primary_host_name: `uname -n`" test_probes_tc02.out || ret_val=1
	for i in `sed -n '6,$p' test_probes_tc02.out | awk '{print $1}'`; do
	    IPV4=`ifconfig $i | sed 's/  /\n/g' | grep "inet " | sed 's/addr://' | awk '{print $2}' | sed 's/\/.*$//'`
	    IPV6=`ifconfig $i | sed 's/  /\n/g' | grep "inet6 " | sed 's/addr://' | awk '{print $2}' | sed 's/\/.*$//'`
	    grep "$IPV4" test_probes_tc02.out | grep -q $i || ret_val=1
	    grep "$IPV6" test_probes_tc02.out | grep -q $i || ret_val=1
	done
    fi

    return $ret_val
}

function test_probes_probe_api {
    local ret_val=0;

    ./test_probe-api > ./test_probes_tc03.out

    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_probes_cleanup {     
    local ret_val=0;    

    rm -f test_probes_tc01.out \
          test_probes_tc02.out \
	  test_probes_tc03.out 

    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_probes.log

exec 2>$log

test_probes_setup   
ret_val=$? 
report_result "test_probes_setup" $ret_val 
result=$[$result+$ret_val]

test_probes_import 
ret_val=$? 
report_result "test_probes_import" $ret_val 
result=$[$result+$ret_val]   

test_probes_get_system_chars
ret_val=$? 
report_result "test_probes_get_system_chars" $ret_val  
result=$[$result+$ret_val]   

test_probes_probe_api
ret_val=$?
report_result "test_probes_probe_api" $ret_val  
result=$[$result+$ret_val]   

test_probes_cleanup
ret_val=$?
report_result "test_probes_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result


