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

function test_probes_tc01 {
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
	${srcdir}/test_probes "${srcdir}/OVAL/${F}" > ${srcdir}/test_probes_tc01.out
	ret_val=$[$ret_val+$?]
    done

    return $ret_val
}

# Check if selected system characteristics were populated correctly. 
function test_probes_tc02 {
    local ret_val=0
   
    ${srcdir}/test_sysinfo 2>&1 > ${srcdir}/test_probes_tc02.out
    ret_val=$?
        
    [ `du ${srcdir}/test_probes_tc02.out | awk '{print $1}'` -eq 0 ] && ret_val=1
    
    if [ $ret_val -eq 0 ]; then
	grep -q "os_name: `uname`" ${srcdir}/test_probes_tc02.out || ret_val=1
	grep -q "os_version: `uname -v`" ${srcdir}/test_probes_tc02.out || ret_val=1
	grep -q "os_architecture: `uname -m`" ${srcdir}/test_probes_tc02.out || ret_val=1
	grep -q "primary_host_name: `uname -n`" ${srcdir}/test_probes_tc02.out || ret_val=1
	for i in `sed -n '6,$p' ${srcdir}/test_probes_tc02.out | awk '{print $1}'`; do
	    IPV4=`ifconfig $i | sed 's/  /\n/g' | grep "inet " | sed 's/addr://' | awk '{print $2}' | sed 's/\/.*$//'`
	    IPV6=`ifconfig $i | sed 's/  /\n/g' | grep "inet6 " | sed 's/addr://' | awk '{print $2}' | sed 's/\/.*$//'`
	    grep "$IPV4" ${srcdir}/test_probes_tc02.out | grep -q $i || ret_val=1
	    grep "$IPV6" ${srcdir}/test_probes_tc02.out | grep -q $i || ret_val=1
	done
    fi

    return $ret_val
}

function test_probes_tc03 {
    local ret_val=0;

    ${srcdir}/test_probe-api > ${srcdir}/test_probes_tc03.out

    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_probes_cleanup {     
    local ret_val=0;    

    rm -f ${srcdir}/test_probes_tc01.out \
          ${srcdir}/test_probes_tc02.out \
	  ${srcdir}/test_probes_tc03.out 

    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=${srcdir}/test_probes.log

exec 2>$log

test_probes_setup   
ret_val=$? 
report_result "test_probes_setup" $ret_val 
result=$[$result+$ret_val]

test_probes_tc01     
ret_val=$? 
report_result "test_probes_tc01" $ret_val 
result=$[$result+$ret_val]   

test_probes_tc02    
ret_val=$? 
report_result "test_probes_tc02" $ret_val  
result=$[$result+$ret_val]   

test_probes_tc03
ret_val=$?
report_result "test_probes_tc03" $ret_val  
result=$[$result+$ret_val]   

test_probes_cleanup
ret_val=$?
report_result "test_probes_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "------------------------------------------"
echo "See ${log}"

exit $result


