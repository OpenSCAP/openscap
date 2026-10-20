#!/usr/bin/env bash

# Copyright 2023 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Test Suite
#
# Authors:
#      Evgenii Kolesnikov <ekolesni@redhat.com>

. $builddir/tests/test_common.sh

# Test cases.

function test_oscap_util {
    ./test_oscap_util
}

# Testing.

test_init

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_oscap_util" test_oscap_util
fi

test_exit
