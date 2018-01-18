#!/usr/bin/env bash

# Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
#      Petr Lautrbach <plautrba@redhat.com>

. $builddir/tests/test_common.sh

# Test Cases.

function test_probes_environmentvariable58 {

    probecheck "environmentvariable58" || return 255

    local ret_val=0;
    local DF="$1.xml"
    local RF="$1.results.xml"

    [ -f $RF ] && rm -f $RF

    bash ${srcdir}/$1.xml.sh > $DF
    LINES=$?

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF $LINES
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

# Testing.

test_init

test_run "test_probes_environmentvariable58" test_probes_environmentvariable58 \
    test_probes_environmentvariable58
test_run "test_probes_environmentvariable58-fail" \
    test_probes_environmentvariable58 test_probes_environmentvariable58-fail

test_exit
