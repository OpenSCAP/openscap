#!/usr/bin/env bash

# Copyright 2018 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap XCCDF Module Test Suite.
#
# Authors:
#      Peter Vrabec <pvrabec@redhat.com>
#      Alexander Scheel <ascheel@redhat.com>

. $builddir/tests/test_common.sh

# Test cases.

function test_known_extension {
	local INPUT=$1

	output=$(bash $builddir/run $builddir/utils/oscap xccdf eval $srcdir/$INPUT 2>&1)
	has_warning=$(echo "$output" | grep -i "Skipping rule")

	return $([ "x$has_warning" == "x" ])
}



function test_malformed_extension {
	local INPUT=$1

	output=$(bash $builddir/run $builddir/utils/oscap xccdf eval $srcdir/$INPUT 2>&1)
	has_warning=$(echo "$output" | grep -i "Skipping rule that uses OVAL")

	return $([ "x$has_warning" != "x" ])
}


function test_unknown_extension {
	local INPUT=$1

	output=$(bash $builddir/run $builddir/utils/oscap xccdf eval $srcdir/$INPUT 2>&1)
	has_warning=$(echo "$output" | grep -i "Skipping rule that requires")

	return $([ "x$has_warning" != "x" ])
}

# Testing.

test_init "test_xccdf_check_extensions.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "known check extension" test_known_extension test_known_extensions.xml
    test_run "malformed check extension" test_malformed_extension test_malformed_extensions.xml
    test_run "unknown check extension" test_unknown_extension test_proprietary_extensions.xml
fi

test_exit
