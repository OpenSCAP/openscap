#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

. ${srcdir}/../../test_common.sh

# Test Cases.

function test_probes_sysinfo {

    if [ ! -x ${OVAL_PROBE_DIR}/probe_system_info ]; then		
	echo -e "Probe sysinfo does not exist!\n" 
	return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local LOGFILE="test_probes_sysinfo.log"

    ./test_probes_sysinfo >> "$LOGFILE"

    if [ $? -eq 0 ] && [ -f $RESFILE ]; then
	
	OS_NAME="`uname -s`"
	if ! grep -q "os_name: $OS_NAME" "$LOGFILE"; then
	    echo "os_name should be `uname -s`" 
	    ret_val=$[$ret_val + 1]
	fi
	
	OS_VERSION="`uname -v`"
	if ! grep -q "os_version: ${OS_VERSION}" "$LOGFILE"; then 
	    echo "os_version should be ${OS_VERSION}" 
	    ret_val=$[$ret_val + 1]
	fi

	OS_ARCHITECTURE="`uname -m`"
	if ! grep -q "os_architecture: ${OS_ARCHITECTURE}" "$LOGFILE"; then 
	    echo "os_architecture should be ${OS_ARCHITECTURE}" 
	    ret_val=$[$ret_val + 1]
	fi

	PRIMARY_HOST_NAME="`uname -n`"
	if ! grep -q "primary_host_name: ${PRIMARY_HOST_NAME}" "$LOGFILE"; then 
	    echo "primary_host_name should be ${PRIMARY_HOST_NAME}" 
	    ret_val=$[$ret_val + 1]
	fi

	# TODO: network interfaces check
	
 	if [ ! $ret_val -eq 0 ]; then
	    echo "" 
	    cat "$LOGFILE" 
	    echo "" 
	    ret_val=2
	fi
    else	
	echo "Unable to probe system characteristics!" 
	echo "" 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_sysinfo.log"

test_run "test_probes_sysinfo" test_probes_sysinfo

test_exit
