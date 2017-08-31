#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CVSS Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#      Ondrej Moris <omoris@redhat.com



. ../../test_common.sh

# Test cases.

# check vector against expected value
function test_api_cvss_vector {
    ./test_api_cvss --base $A $B $C $D $E $F >/dev/null

    local ret vector value v
    ret=0

    while read vector value; do
        v=`./test_api_cvss $vector`
        [ "$value" != "$v" ] && ret=1
        echo "$vector --> $v ($value)"
    done <vectors.txt

    return $ret
}

# Testing.

test_init "test_api_cvss.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_api_cvss_vector" test_api_cvss_vector
fi

test_exit 

