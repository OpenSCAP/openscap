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

function test_probes_filehash {

    probecheck "filehash" || return 255
    require "sha1sum" || return 255
    require "md5sum" || return 255

    local ret_val=0;
    local DF="test_probes_filehash.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    bash ${srcdir}/test_probes_filehash.xml.sh > $DF
    $OSCAP oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 120
	ret_val=$?
    else 
	ret_val=1
    fi

    [ $ret_val -eq 0 ] && rm -f /tmp/test_probes_filehash.tmp

    return $ret_val
}

# Testing.

test_init "test_probes_filehash.log"

test_run "test_probes_filehash" test_probes_filehash

test_exit
