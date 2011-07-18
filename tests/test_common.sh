#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Testing Helpers.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

# Normalized path.
PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin

# Overall test result.
result=0

# Logging file (stderr is redirected here).
log=test.log

# Set-up testing environment.
function test_init {
    [ $# -eq 1 ] && log=$1
    exec 2>$log
    echo ""
    echo "--------------------------------------------------"
}

# Execute test and report its results.
function test_run {    
    printf "+ %-40s" $1; 
    echo -e "TEST: $1" >&2; 
    shift
    ( exec 1>&2 ; eval "$@" )
    ret_val=$?
    if [ $ret_val -eq 0 ]; then 
	echo "[ PASS ]"; 
	echo -e "RESULT: PASSED\n" >&2
	return 0;
    elif [ $ret_val -eq 1 ]; then
        result=$[$result + $ret_val]
	echo "[ FAIL ]"; 
	echo -e "RESULT: FAILED\n" >&2
	return 1;
    elif [ $ret_val -eq 255 ]; then
	echo "[ SKIP ]"; 
	echo -e "RESULT: SKIPPED\n" >&2
	return 0; 
    else
        result=$[$result + $ret_val]
	echo "[ WARN ]"; 
	echo -e "RESULT: WARNING (unknown exist status $ret_val)\n" >&2
	return 1;
    fi    
}

# Clean-up testing environment.
function test_exit {
    echo "--------------------------------------------------"
    echo -e "See `pwd | sed 's|.*/\(tests/.*\)|\1|'`/${log}.\n"

    if [ $# -eq 1 ]
    then
        ( exec 1>&2 ; eval "$@" )
    fi

    [ $result -eq 0 ] && exit 0
    exit 1
}
