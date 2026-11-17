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
    output=$(./test_probe_xinetd "${srcdir}/xinetd_H.conf" telnet tcp | grep 'xiconf_service_t(telnet)' | wc -l | xargs)

    if [ "$output" = "3" ]; then
	return 0
    fi
    return 1
}

# Regression test for memory-safety bugs in xiconf_parse()/xiconf_parse_section().
# Each of these inputs used to crash the parser (heap-buffer-overflow, NULL-pointer
# dereference, invalid free, ...) before the guards were added. The parser must now
# handle them without crashing; the service/protocol arguments are arbitrary, we only
# care that the process is neither killed by a signal nor trips a sanitizer.
function test_probe_xinetd_regression_malformed {
    local ret_val=0
    local f errf rc
    for f in xinetd_crash_keyword_no_value.conf \
	     xinetd_crash_section_no_eol.conf \
	     xinetd_crash_service_name_no_space.conf \
	     xinetd_crash_service_null_protocol.conf \
	     xinetd_crash_type_not_enum.conf ; do
	errf=$(mktemp)
	./test_probe_xinetd "${srcdir}/${f}" foo tcp > /dev/null 2>"$errf"
	rc=$?
	# Ignore the pre-existing, harmless UBSan report about calling a typed
	# rbtree callback through a generic function pointer; flag a termination
	# by signal (exit >= 128) or any other sanitizer/runtime error.
	if [[ "$rc" -ge 128 ]] || \
	   grep -v "through pointer to incorrect function type" "$errf" \
		| grep -q "ERROR: AddressSanitizer:\|AddressSanitizer:DEADLYSIGNAL\|LeakSanitizer:\|runtime error:" ; then
	    echo "CRASH on malformed input ${f} (exit ${rc}):"
	    cat "$errf"
	    ret_val=$[$ret_val + 1]
	fi
	rm -f "$errf"
    done
    return $ret_val
}

# Testing.

test_init

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_probe_xinetd_parser" test_probe_xinetd_parser
    test_run "xinetd parser regression test: string list" test_probe_xinetd_regression_stringlist
    test_run "test_probe_xinetd_duplicates" test_probe_xinetd_duplicates
    test_run "xinetd parser regression test: malformed input" test_probe_xinetd_regression_malformed
fi

test_exit
