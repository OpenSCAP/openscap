#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Testing Helpers.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

# Reporting results.
function report_result {    

    printf "+ %-40s" ${1}; 
    if [ $2 -eq 0 ]; then 
	echo "[ PASS ]"; printf "*** %s PASSED ***\n\n" ${1} >&2
    else 
	echo "[ FAIL ]"; printf "*** %s FAILED ***\n\n" ${1} >&2
    fi
}

# Compare content of two XML files.
function xml_cmp {
    local ret_val=0

    /usr/bin/perl -w ${srcdir}/xmldiff.pl $1 $2 
    ret_val=$?

    return $ret_val
}

