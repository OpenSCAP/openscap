#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap XCCDF Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#      Lukas Kuklinek <lkuklinek@redhat.com>
#      Ondrej Moris <omoris@redhat.com>


. $srcdir/../../test_common.sh

# Test cases.

function test_api_xccdf11_export {
    require "egrep" || return 255
    local ret_val=0

    ./test_api_xccdf --export $srcdir/scap-rhel5-xccdf11.xml \
	scap-rhel5-xccdf11.xml.out
    
    if [ $? -eq 0 ]; then
	local IGNORE_ATTRS="weight|schemaLocation|lang"
	$srcdir/../../xmldiff.pl $srcdir/scap-rhel5-xccdf11.xml \
	    scap-rhel5-xccdf11.xml.out | \
	    egrep -v "[aA]ttribute '($IGNORE_ATTRS)'"
	if [ $? -neq 0 ]; then
	    echo "Exported file differs from what is expected!"
	    ret_val=1
	fi
    else
	echo "Cannot export xccdf content!"
	ret_val=1
    fi

    return $ret_val
}

function test_api_xccdf12_export {
    require "egrep" || return 255
    local ret_val=0

    ./test_api_xccdf --export $srcdir/scap-rhel5-xccdf12.xml \
        scap-rhel5-xccdf12.xml.out

    if [ $? -eq 0 ]; then
        local IGNORE_ATTRS="weight|schemaLocation|lang"
        $srcdir/../../xmldiff.pl $srcdir/scap-rhel5-xccdf12.xml \
            scap-rhel5-xccdf12.xml.out | \
            egrep -v "[aA]ttribute '($IGNORE_ATTRS)'"
        if [ $? -neq 0 ]; then
            echo "Exported file differs from what is expected!"
            ret_val=1
        fi
    else
        echo "Cannot export xccdf content!"
        ret_val=1
    fi

    return $ret_val
}

function text_api_xccdf_import {
    ./test_api_xccdf_dump $srcdir/scap-rhel5-xccdf.xml
    return $?
}

# Testing.

test_init "test_api_xccdf.log"

#test_run "text_api_xccdf_import" text_api_xccdf_import
test_run "text_api_xccdf11_export" test_api_xccdf11_export
test_run "text_api_xccdf12_export" test_api_xccdf12_export

test_exit
