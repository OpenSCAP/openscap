#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Test Suite
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

. ../../../test_common.sh

# Test cases.

set -e -o pipefail

TEST="./test_schema_version"

function test_comparing_schema_versions {
# format: version_1 version_2 result
    $TEST 5.2 5.2 0
    $TEST 4.2 5.2 -1
    $TEST 6.2 4.2 1
    $TEST 5.10 5.11 -1
    $TEST 5.11 5.10 1
    $TEST 6.0.1 6.0 1
    $TEST 6.0.1 6.0.2 -1
    $TEST 6.0.2 6.1.2 -1
    $TEST 6.13.4 6.13 1
    $TEST 999.999.999 999.999.998 1
    $TEST 5.11.1:1.0 5.11.1:1.0 0
    $TEST 5.11.1:1.1 5.11.1:1.2 -1
    $TEST 5.11.1:2.1 5.11.1:1.2 1
    $TEST 5.11.2:1.0 5.11.1:1.0 1
    $TEST 5.11.2:1.1 5.12.1:1.1 -1
    $TEST 6.3:8.7.9 6.3:8.7.10 -1
    $TEST 13.0:1.2.3 13.0:1.3 -1
    $TEST 13.0:1.2.3 13.0:1.2 1
    $TEST 42.42.42:42.42.42 42.42.42:42.42.42 0
}

# Testing.

test_init "test_schema_version.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_comparing_schema_versions" test_comparing_schema_versions
fi

test_exit
