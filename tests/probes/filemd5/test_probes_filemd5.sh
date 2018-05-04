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

. "$builddir/tests/test_common.sh"

# Test Cases.

function test_probes_filemd5 {

    probecheck "filemd5" || return 255
    require "md5sum" || return 255

    local ret_val=0;
    local DEFFILE="test_probes_filemd5.xml"
    local RESFILE="results.xml"

    [ -f $RESFILE ] && rm -f $RESFILE

    bash ${srcdir}/test_probes_filemd5.xml.sh > $DEFFILE
    $OSCAP oval eval --results $RESFILE $DEFFILE
    
    if [ -f $RESFILE ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 96
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init "test_probes_filemd5.log"

test_run "test_probes_filemd5" test_probes_filemd5

test_exit
