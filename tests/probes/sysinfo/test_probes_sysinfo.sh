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

. $builddir/tests/test_common.sh

# Test Cases.

function test_probes_sysinfo {

    probecheck "system_info" || return 255

    local ret_val=0;
    local LOGFILE="test_probes_sysinfo.log"

    bash $builddir/run ./test_probes_sysinfo >> "$LOGFILE"

    if [ $? -eq 0 ] && [ -f $RF ]; then
	
	OS_NAME="`uname -s`"
	if ! grep "os_name: $OS_NAME" "$LOGFILE" > /dev/null; then
	    echo "os_name should be `uname -s`" 
	    ret_val=$[$ret_val + 1]
	fi
	
	OS_VERSION="`uname -v`"
	if ! grep "os_version: ${OS_VERSION}" "$LOGFILE" > /dev/null; then 
	    echo "os_version should be ${OS_VERSION}" 
	    ret_val=$[$ret_val + 1]
	fi

	OS_ARCHITECTURE="`uname -m`"
	if ! grep "os_architecture: ${OS_ARCHITECTURE}" "$LOGFILE" > /dev/null; then 
	    echo "os_architecture should be ${OS_ARCHITECTURE}" 
	    ret_val=$[$ret_val + 1]
	fi

	PRIMARY_HOST_NAME="`uname -n`"
	if ! grep "primary_host_name: ${PRIMARY_HOST_NAME}" "$LOGFILE" > /dev/null; then 
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

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_probes_sysinfo" test_probes_sysinfo
fi

test_exit
