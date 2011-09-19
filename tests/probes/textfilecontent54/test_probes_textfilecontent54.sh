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

function test_probes_textfilecontent54 {

    probecheck "textfilecontent54" || return 255
	
    local ret_val=0;
    local DF="${srcdir}/test_probes_textfilecontent54.xml"
    local RF="results.xml"
    
    [ -f $RF ] && rm -f $RF

    local FILE_A="/tmp/test_probes_textfilecontent54.tmp_file"
    local FILE_B="/tmp/test_probes_textfilecontent54.tmp_file_empty"
    local FILE_C="/tmp/test_probes_textfilecontent54.tmp_file_non_existing"

    touch "$FILE_A"
    touch "$FILE_B"

    echo "valid_key = valid_value" > "$FILE_A"
    echo "valid_key = valid_value" >> "$FILE_A"

    ../../../utils/.libs/oscap oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 16
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_textfilecontent54.log"

test_run "test_probes_textfilencontent54" test_probes_textfilecontent54

test_exit
