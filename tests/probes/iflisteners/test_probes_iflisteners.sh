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

. $builddir/tests/test_common.sh

# Test Cases.

function test_probes_iflisteners {

    probecheck "iflisteners" || return 255

    if [ `id -u` -ne 0 ]; then
        echo you need to be root
        exit 255
    fi

    local ret_val=0;
    local DEFFILE="$1.xml"
    local RESFILE="$1.results.xml"

    [ -f $RESFILE ] && rm -f $RESFILE

    bash ${srcdir}/$1.xml.sh > $DEFFILE
    LINES=$?

    $OSCAP oval eval --results $RESFILE $DEFFILE
    
    if [ -f $RESFILE ]; then
	verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF $LINES
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

# Testing.
test_init "test_probes_iflisteners.log"

tcpdump -i lo -p tcp port 12345 > /dev/null 2> /dev/null &
sleep 3
pid=$!


test_run "test_probes_iflisteners" test_probes_iflisteners test_probes_iflisteners

kill $pid

test_exit
