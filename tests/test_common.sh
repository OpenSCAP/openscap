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
    printf "+ %-32s" ${1}; echo -n "${1} " >&2
    if [ $2 -eq 0 ]; then 
	echo "[ PASS ]"; echo "PASS" >&2
    else 
	echo "[ FAIL ]"; echo "FAIL" >&2
    fi
}

# Compare content of two XML files.
function xml_cmp {
    local ret_val=0

    /usr/bin/perl -w ./xmldiff.pl $1 $2 
    ret_val=$?

    return $ret_val
}

