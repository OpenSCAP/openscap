#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap XCCDF Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#

. ${srcdir}/test_common.sh

# Setup.
function test_xccdf_setup {
    local ret_val=0    
    return $ret_val
}

# Test Cases.

function test_xccdf_import {
    local ret_val=0

    ./xccdf_dump "${srcdir}/XCCDF/scap-rhel5-xccdf.xml" > test_xccdf.out 2>&1
    ret_val=$?

    return $ret_val
}

function test_xccdf_export {
	./test_xccdf --export "${srcdir}/XCCDF/scap-rhel5-xccdf.xml" "scap-rhel5-xccdf.out.xml" > test_xccdf_export.out 2>&1 || return 1
	local IGNORE_ATTRS="weight|schemaLocation|lang"
	xml_cmp "${srcdir}/XCCDF/scap-rhel5-xccdf.xml" "scap-rhel5-xccdf.out.xml" 2>&1 | egrep -v "[aA]ttribute '($IGNORE_ATTRS)'" >&2
	[ $? == 0 ] && return 1 || return 0
}

# Cleanup.
function test_xccdf_cleanup {
    local ret_val=0

    rm test_xccdf.out
    ret_val=$?
    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_xccdf.log

exec 2>$log

execute_test xccdf_setup
execute_test xccdf_import
execute_test xccdf_export
execute_test xccdf_cleanup

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result
