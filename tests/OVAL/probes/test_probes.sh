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
    local TEMPDIR="$(mktemp -d -t -q tmp.XXXXXX)"
    local LOGFILE="test_probes_import.out"
    local EXECDIR="$(pwd)"
    local DEFFILE="${EXECDIR}/OVAL/probes/scap-rhel5-oval.xml"    

    pushd "$TEMPDIR" > /dev/null

    # eval "\"${EXECDIR}/test_probes\" \"--parse\" \"$DEFFILE\" " >> "$LOGFILE"
    # ret_val=$?

    popd > /dev/null

    cp "$TEMPDIR/$LOGFILE" .

    return $ret_val
}

# Check if selected system characteristics were populated correctly. 
function test_probes_sysinfo {
    local ret_val=0;
    local LOGFILE="test_probes_sysinfo.out"
    local EXECDIR="$(pwd)"

    eval "\"${EXECDIR}/test_sysinfo\"" >> "$LOGFILE"

    if [ $? -eq 0 ]; then 
	
	OS_NAME="`uname -s`"
	if ! grep -q "os_name: $OS_NAME" "$LOGFILE"; then
	    echo "os_name should be `uname -s`" >&2
	    ret_val=$[$ret_val + 1]
	fi
	
	OS_VERSION="`uname -v`"
	if ! grep -q "os_version: ${OS_VERSION}" "$LOGFILE"; then 
	    echo "os_version should be ${OS_VERSION}" >&2
	    ret_val=$[$ret_val + 1]
	fi

	OS_ARCHITECTURE="`uname -m`"
	if ! grep -q "os_architecture: ${OS_ARCHITECTURE}" "$LOGFILE"; then 
	    echo "os_architecture should be ${OS_ARCHITECTURE}" >&2
	    ret_val=$[$ret_val + 1]
	fi

	PRIMARY_HOST_NAME="`uname -n`"
	if ! grep -q "primary_host_name: ${PRIMARY_HOST_NAME}" "$LOGFILE"; then 
	    echo "primary_host_name should be ${PRIMARY_HOST_NAME}" >&2
	    ret_val=$[$ret_val + 1]
	fi

	# TODO: network interfaces check
	
 	if [ ! $ret_val -eq 0 ]; then
	    echo "" >&2
	    cat "$LOGFILE" >&2
	    echo "" >&2
	    ret_val=2
	fi
    else	
	echo "Unable to probe system characteristics!" >&2
	echo "" >&2
	ret_val=1
    fi

    return $ret_val
}

function test_probes_api {
    local ret_val=0;

    ./test_probe-api > ./test_probes_tc03.out

    ret_val=$?

    return $ret_val
}

function test_probes_file {
    local ret_val=0;
    local LOGFILE="test_probes_file.out"
    local EXECDIR="$(pwd)"
    local DEFFILE="${srcdir}/OVAL/probes/test_probes_file.xml"
    local RESFILE="test_probes_file.xml.results.xml"

    eval "\"${EXECDIR}/test_probes\" \"$DEFFILE\" \"$RESFILE\"" >> "$LOGFILE"

    if [ $? -eq 0 ] && [ -e $RESFILE ]; then

	COUNT=13; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    DEF_DEF=`cat "$DEFFILE" | grep "id=\"definition:${ID}\""`
	    DEF_RES=`cat "$RESFILE" | grep "definition_id=\"definition:${ID}\""`

	    if (echo $DEF_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $DEF_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $DEF_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $DEF_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of definition:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi
	    
	    ID=$[$ID+1]
	done

	COUNT=84; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${ID}\""`

	    if (echo $TEST_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $TEST_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of test:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi
	    
	    ID=$[$ID+1]
	done

	if [ ! $ret_val -eq 0 ]; then
	    echo "" >&2
	    cat "$RESFILE" >&2
	    echo "" >&2
	    ret_val=2
	fi

    else 
	ret_val=1
    fi

    [ -e $RESFILE ] && rm -f "$RESFILE"
    
    return $ret_val
}

function test_probes_rpminfo {
    local ret_val=0;
    local LOGFILE="test_probes_rpminfo.out"
    local EXECDIR="$(pwd)"
    local DEFFILE="test_probes_rpminfo.xml"
    local RESFILE="test_probes_rmpinfo.xml.results.xml"

    eval "which rpm > /dev/null"    
    if [ ! $? -eq 0 ]; then	
	return 255; # Test is not applicable.
    fi

    local RPM_A=`rpm -qa | sed -n '1p'`
    local RPM_B=`rpm -qa | sed -n '2p'`

    EXISTING_RPM_PACKAGE_A=`rpm -qi ${RPM_A} | sed -n '1p' | awk '{ print $3 }'`
    EXISTING_RPM_PACKAGE_B=`rpm -qi ${RPM_B} | sed -n '1p' | awk '{ print $3 }'`

    NON_EXISTING_RPM_PACKAGE_A="non_existing_package_A"
    NON_EXISTING_RPM_PACKAGE_B="non_existing_package_B"

    EXISTING_RPM_VERSION=`rpm -qi ${RPM_A} | sed -n '2p' | awk '{ print $3 }'`
    EXISTING_RPM_VERSION_PATTERN="^[:digit:].*"

    NON_EXISTING_RPM_VERSION="1000.1000"
    NON_EXISTING_RPM_VERSION_PATTERN="^X[^[X]]"

    bash "${srcdir}/OVAL/probes/test_probes_rpminfo.xml.sh" \
	 "$EXISTING_RPM_PACKAGE_A"                          \
	 "$NON_EXISTING_RPM_PACKAGE_A"                      \
	 "$EXISTING_RPM_PACKAGE_B"                          \
	 "$NON_EXISTING_RPM_PACKAGE_B"                      \
	 "$EXISTING_RPM_VERSION"                            \
	 "$EXISTING_RPM_VERSION_PATTERN"                    \
	 "$NON_EXISTING_RPM_VERSION"                        \
	 "$NON_EXISTING_RPM_VERSION_PATTERN" > "$DEFFILE"
    
    eval "\"${EXECDIR}/test_probes\" \"$DEFFILE\" \"$RESFILE\"" >> "$LOGFILE"
    
    if [ $? -eq 0 ] && [ -e $RESFILE ]; then

	COUNT=15; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    DEF_DEF=`cat "$DEFFILE" | grep "id=\"definition:${ID}\""`
	    DEF_RES=`cat "$RESFILE" | grep "definition_id=\"definition:${ID}\""`

	    if (echo $DEF_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $DEF_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $DEF_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $DEF_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of definition:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi

	    ID=$[$ID+1]
	done

	COUNT=49; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${ID}\""`

	    if (echo $TEST_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $TEST_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of test:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi

	    ID=$[$ID+1]
	done

	if [ ! $ret_val -eq 0 ]; then
	    echo "" >&2
	    cat "$RESFILE" >&2
	    echo "" >&2
	    ret_val=2
	fi

    else 
	ret_val=1
    fi

    [ -e $RESFILE ] && rm -f "$RESFILE"
    
    return $ret_val
}

function test_probes_runlevel_A {
    local ret_val=0;
    local LOGFILE="test_probes_runlevel_A.out"
    local EXECDIR="$(pwd)"
    local DEFFILE="test_probes_runlevel_A.xml"
    local RESFILE="test_probes_runlevel_A.xml.results.xml"
   
    eval "which chkconfig > /dev/null"    
    if [ ! $? -eq 0 ]; then	
	return 255; # Test is not applicable.
    fi

    eval "bash \"${srcdir}/OVAL/probes/test_probes_runlevel_A.xml.sh\"" > "$DEFFILE"
    
    eval "\"${EXECDIR}/test_probes\" \"$DEFFILE\" \"$RESFILE\"" >> "$LOGFILE"
    
    if [ $? -eq 0 ] && [ -e $RESFILE ]; then

	for S in `chkconfig --list | awk '{print $1}'`; do
	    for L in `chkconfig $S --list | awk '{print $2 " " $3 " " $4 " " $5 " " $6 " " $7 " " $8}'`; do
		LEVEL=`echo $L | awk -F : '{print $1}'`
		STATE=`echo $L | awk -F : '{print $2}'`
	    
		for SUFFIX in "T F"; do
		    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${S}-${LEVEL}-${STATE}-${SUFFIX}"`
		    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${S}-${LEVEL}-${STATE}-${SUFFIX}\""`

		    if (echo $TEST_RES | grep -q "result=\"true\""); then
			RES="TRUE"
		    elif (echo $TEST_RES | grep -q "result=\"false\""); then
			RES="FALSE"
		    else
			RES="ERROR"
		    fi
		    
		    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
			CMT="TRUE"
		    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
			CMT="FALSE"
		    else
			CMT="ERROR"
		    fi
		    
		    if [ ! $RES = $CMT ]; then
			echo "Result of test:${S}-${LEVEL}-${STATE}-${SUFFIX} should be ${CMT}!" >&2
			ret_val=$[$ret_val + 1]
		    fi
		    
		done
	    done
	done

	if [ ! $ret_val -eq 0 ]; then
	    echo "" >&2
	    cat "$RESFILE" >&2
	    echo "" >&2
	    ret_val=2
	fi
	
    else 
	ret_val=1
    fi

    [ -e $RESFILE ] && rm -f "$RESFILE"
    
    return $ret_val
}

function test_probes_runlevel_B {
    local ret_val=0;
    local LOGFILE="test_probes_runlevel_B.out"
    local EXECDIR="$(pwd)"
    local DEFFILE="test_probes_runlevel_B.xml"
    local RESFILE="test_probes_runlevel_B.xml.results.xml"
    
    eval "which chkconfig > /dev/null"    
    if [ ! $? -eq 0 ]; then	
	return 255; # Test is not applicable.
    fi

    local SERVICE_A=`chkconfig --list | grep "3:on" | head -1 | awk '{print $1}'`
    local SERVICE_B=`chkconfig --list | grep "3:off" | head -1 | awk '{print $1}'`

    eval "bash \"${srcdir}/OVAL/probes/test_probes_runlevel_B.xml.sh\" \
	       \"$SERVICE_A\"                                          \
               \"$SERVICE_B\"" > "$DEFFILE"
    
    eval "\"${EXECDIR}/test_probes\" \"$DEFFILE\" \"$RESFILE\"" >> "$LOGFILE"
    
    if [ $? -eq 0 ] && [ -e $RESFILE ]; then

	COUNT=13; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    DEF_DEF=`cat "$DEFFILE" | grep "id=\"definition:${ID}\""`
	    DEF_RES=`cat "$RESFILE" | grep "definition_id=\"definition:${ID}\""`

	    if (echo $DEF_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $DEF_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $DEF_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $DEF_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of definition:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi

	    ID=$[$ID+1]
	done

	COUNT=40; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${ID}\""`

	    if (echo $TEST_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $TEST_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of test:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi
	    
	    ID=$[$ID+1]
	done

	if [ ! $ret_val -eq 0 ]; then
	    echo "" >&2
	    cat "$RESFILE" >&2
	    echo "" >&2
	    ret_val=2
	fi

    else 
	ret_val=1
    fi

    [ -e $RESFILE ] && rm -f "$RESFILE"
    
    return $ret_val
}

function test_probes_textfilecontent54 {
    local ret_val=0;
    local LOGFILE="test_probes_textfilecontent54.out"
    local EXECDIR="$(pwd)"
    local DEFFILE="${srcdir}/OVAL/probes/test_probes_textfilecontent54.xml"
    local RESFILE="test_probes_textfilecontent54.xml.results.xml"
    
    local FILE_A="/tmp/test_probes_textfilecontent54.tmp_file"
    local FILE_B="/tmp/test_probes_textfilecontent54.tmp_file_empty"
    local FILE_C="/tmp/test_probes_textfilecontent54.tmp_file_non_existing"

    touch "$FILE_A"
    touch "$FILE_B"

    echo "valid_key = valid_value" > "$FILE_A"
    echo "valid_key = valid_value" >> "$FILE_A"

    eval "\"${EXECDIR}/test_probes\" \"$DEFFILE\" \"$RESFILE\"" >> "$LOGFILE"
    
    if [ $? -eq 0 ] && [ -e $RESFILE ]; then

	COUNT=13; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    DEF_DEF=`cat "$DEFFILE" | grep "id=\"definition:${ID}\""`
	    DEF_RES=`cat "$RESFILE" | grep "definition_id=\"definition:${ID}\""`

	    if (echo $DEF_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $DEF_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $DEF_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $DEF_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of definition:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi

	    ID=$[$ID+1]
	done

	COUNT=16; ID=1
	while [ $ID -le $COUNT ]; do
	    
	    TEST_DEF=`cat "$DEFFILE" | grep "id=\"test:${ID}\""`
	    TEST_RES=`cat "$RESFILE" | grep "test_id=\"test:${ID}\""`

	    if (echo $TEST_RES | grep -q "result=\"true\""); then
		RES="TRUE"
	    elif (echo $TEST_RES | grep -q "result=\"false\""); then
		RES="FALSE"
	    else
		RES="ERROR"
	    fi

	    if (echo $TEST_DEF | grep -q "comment=\"true\""); then
		CMT="TRUE"
	    elif (echo $TEST_DEF | grep -q "comment=\"false\""); then
		CMT="FALSE"
	    else
		CMT="ERROR"
	    fi

	    if [ ! $RES = $CMT ]; then
		echo "Result of test:${ID} should be ${CMT}!" >&2
		ret_val=$[$ret_val + 1]
	    fi
	    
	    ID=$[$ID+1]
	done

	if [ ! $ret_val -eq 0 ]; then
	    echo "" >&2
	    cat "$RESFILE" >&2
	    echo "" >&2
	    ret_val=2
	fi

    else 
	ret_val=1
    fi

    [ -e $RESFILE ] && rm -f "$RESFILE"
    
    return $ret_val
}

# Cleanup.
function test_probes_cleanup {     
    local ret_val=0;    

    rm -f "test_probes_*.out"

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

# test_probes_import 
# ret_val=$? 
# report_result "test_probes_import" $ret_val 
# result=$[$result+$ret_val]   

test_probes_sysinfo
ret_val=$? 
report_result "test_probes_sysinfo" $ret_val  
result=$[$result+$ret_val]   

# test_probes_api
# ret_val=$?
# report_result "test_probes_api" $ret_val  
# result=$[$result+$ret_val]   

test_probes_file
ret_val=$?
report_result "test_probes_file" $ret_val  
result=$[$result+$ret_val]   

test_probes_rpminfo
ret_val=$?
report_result "test_probes_rpminfo" $ret_val  
result=$[$result+$ret_val]   

test_probes_runlevel_A
ret_val=$?
report_result "test_probes_runlevel_A" $ret_val  
result=$[$result+$ret_val]   

test_probes_runlevel_B
ret_val=$?
report_result "test_probes_runlevel_B" $ret_val  
result=$[$result+$ret_val]   

test_probes_textfilecontent54
ret_val=$?
report_result "test_probes_textfilecontent54" $ret_val  
result=$[$result+$ret_val]   

test_probes_cleanup
ret_val=$?
report_result "test_probes_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result


