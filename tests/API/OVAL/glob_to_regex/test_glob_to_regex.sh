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

function test_glob_to_regex {
    ./test_glob_to_regex
}

# Testing.

test_init "test_glob_to_regex.log"
test_run "test_glob_to_regex" test_glob_to_regex
test_exit
