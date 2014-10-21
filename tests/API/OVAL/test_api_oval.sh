#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap OVAL Module Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      Peter Vrabec, <pvrabec@redhat.com>
#      Ondrej Moris, <omoris@redhat.com>

. ../../test_common.sh


# Test Cases.

function test_api_oval_definition {
    ./test_api_oval ${srcdir}/scap-rhel5-oval.xml
}

function test_api_oval_syschar {
    ./test_api_syschar $srcdir/composed-oval.xml \
	$srcdir/system-characteristics.xml
}

function test_api_oval_results {
    ./test_api_results $srcdir/results.xml exported-results.xml
    cmp $srcdir/results-good.xml exported-results.xml
}

function test_api_oval_directives {
    ./test_api_directives $srcdir/directives.xml exported-directives.xml
    cmp $srcdir/directives.xml exported-directives.xml
}

# Testing.

test_init "test_api_oval.log"

test_run "test_api_oval_definition" test_api_oval_definition
test_run "test_api_oval_syschar" test_api_oval_syschar
test_run "test_api_oval_results" test_api_oval_results
test_run "test_api_oval_directives" test_api_oval_directives

test_exit
