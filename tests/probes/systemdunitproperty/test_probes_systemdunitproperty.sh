#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. ../../test_common.sh

function test_probes_systemdunitproperty {
    probecheck "systemdunitproperty" || return 255
    pidof systemd > /dev/null || return 255

    local DF="${srcdir}/test_probes_systemdunitproperty.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 16
        ret_val=$?
    else
        ret_val=1
    fi

    return $ret_val
}

test_probes_systemdunitproperty
