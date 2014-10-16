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

. ../../test_common.sh

# Test Cases.

function test_probe_xinetd_parser {
    local ret_val=0;

    ./test_probe_xinetd ${srcdir}/xinetd_A.conf foo tcp 
    if [ $? -ne 3 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_B.conf foo tcp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_C.conf a tcp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_D.conf f udp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_E.conf foo udp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_E.conf foo tcp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_F.conf foo udp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    ./test_probe_xinetd ${srcdir}/xinetd_F.conf foo tcp 
    if [ $? -ne 0 ]; then
	ret_val=$[$ret_val + 1]
    fi

    return $ret_val
}

function test_probe_xinetd_regression_stringlist {
    output=$(./test_probe_xinetd "${srcdir}/xinetd_G.conf" rsync tcp | sed -n 's|.*only_from:[[:space:]]*\(only_this_is_allowed\)[[:space:]]*$|\1|p')

    if [ "$output" = "only_this_is_allowed" ]; then
	return 0
    fi
    return 1
}

function test_probe_xinetd_duplicates {
    output=$(./test_probe_xinetd "${srcdir}/xinetd_H.conf" telnet tcp | grep 'xiconf_service_t(telnet)' | wc -l)

    if [ "$output" = "3" ]; then
	return 0
    fi
    return 1
}

# Testing.

test_init "test_probe_xinetd.log"

test_run "test_probe_xinetd_parser" test_probe_xinetd_parser
test_run "xinetd parser regression test: string list" test_probe_xinetd_regression_stringlist
test_run "test_probe_xinetd_duplicates" test_probe_xinetd_duplicates

test_exit
