#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap SEAP Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      David Niemoller
#      Ondrej Moris <omoris@redhat.com>

. $builddir/tests/test_common.sh

# Test cases.

function test_api_seap_concurency {
    local ret_val=0;

    export SEXP_VALIDATE_DISABLE="1"
    export SEAP_DEBUGLOG_DISABLE="1"
    ./test_api_seap_concurency
    ret_val=$?
    unset SEXP_VALIDATE_DISABLE
    unset SEAP_DEBUGLOG_DISABLE

    return $ret_val
}

function test_api_strto {
    ./test_api_strto
}

# Testing.

test_init

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_api_seap_concurency"           test_api_seap_concurency
    test_run "test_api_seap_spb"                  ./test_api_seap_spb
    test_run "test_api_seap_list"                 ./test_api_seap_list
    test_run "test_api_seap_number_expression"    ./test_api_seap_number
    test_run "test_api_seap_string_expression"    ./test_api_seap_string
    test_run "test_api_SEXP_deepcmp"              ./test_api_SEXP_deepcmp
    test_run "test_api_strto"                     ./test_api_strto
fi

test_exit
