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

# Reporting results.
function report_result {    

    printf "+ %-40s" ${1}; 
    if [ $2 -eq 0 ]; then 
	echo "[ PASS ]"; printf "*** %s PASSED ***\n\n" ${1} >&2
	return 0;
    elif [ $2 -eq 255 ]; then
	echo "[ SKIP ]"; printf "*** %s SKIPPED ***\n\n" ${1} >&2
	return 0;
    else
	echo "[ FAIL ]"; printf "*** %s FAILED ***\n\n" ${1} >&2
	return 1;
    fi
}

# Compare content of two XML files.
function xml_cmp {
    local ret_val=0

    /usr/bin/perl -w ${srcdir}/xmldiff.pl $1 $2 
    ret_val=$?

    return $ret_val
}

function execute_test {
	"test_$1"
	ret_val=$? 
	report_result "test_$1" $ret_val
	result=$[$result+$ret_val]
}

