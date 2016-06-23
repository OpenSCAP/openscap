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

. ../../test_common.sh

# Test Cases.

function test_probes_file {

    probecheck "file" || return 255

    local ret_val=0;
    local DF="$srcdir/test_probes_file.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

	if [ "$1" == "verbose" ];
	then
		VF="verbose"
		$OSCAP oval eval --verbose DEVEL --verbose-log-file $VF --results $RF $DF
	else
		$OSCAP oval eval --results $RF $DF
	fi

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 204
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_file.log"

test_run "test_probes_file with verbose mode" test_probes_file verbose
test_run "test_probes_file" test_probes_file

test_exit
