#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CvE Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#      Ondrej Moris <omoris@redhat.com>

. $srcdir/../../test_common.sh

# Test cases.

function test_api_cve_smoke {
    ./test_api_cve --sanity-check 
    return $?
}

function test_api_cve_cvss {
     ./test_api_cve --test-cvss $srcdir/cve_cvss.xml "UTF-8"
     return $?
}

function test_api_cve_export {
    local ret_val=0
    ./test_api_cve --test-export-all $srcdir/cve.xml "UTF-8" export.out "UTF-8"
    if [ $? -eq 0 ] && [ -f export.out ]; then
 	if ! $srcdir/../../xmldiff.pl $srcdir/cve.xml export.out; then
	    echo "Exported file differs from what is expected!"
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi
    return $ret_val
}

function test_api_cve_add_entry {
    local ret_val=0
    ./test_api_cve --add-entries export.out "UTF-8" "CVE-01" "CVE-02" "CVE-03"
    if [ $? -eq 0 ] && [ -f export.out ]; then
	if ! $srcdir/../../xmldiff.pl $srcdir/cve.xml export.out; then
	    echo "Exported file differs from what is expected!"
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi
    return $ret_val
}

function test_api_cve_export_all {
    require "egrep" || return 255
    local ret_val=0
    ./test_api_cve --test-export-all $srcdir/nvdcve-2.0-recent.xml "UTF-8" \
	export.out "UTF-8"
    $srcdir/../../xmldiff.pl $srcdir/nvdcve-2.0-recent.xml export.out | \
	egrep -v "[aA]ttribute 'lang'.*'references'|'schemaLocation'"
    ret_val=$?
    return $ret_val
}

# Testing.

test_init "test_api_cve.log"

test_run "test_api_cve_smoke"       test_api_cve_smoke
test_run "test_api_cve_cvss"        test_api_cve_cvss
test_run "test_api_cve_add_entry"   test_api_cve_add_entry
test_run "test_api_cve_export"      test_api_cve_export
#test_run "test_api_cve_export_all"  test_api_cve_export_all

test_exit
