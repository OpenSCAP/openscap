#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. "$builddir/tests/test_common.sh"

function test_probes_systemdunitproperty {
    probecheck "systemdunitproperty" || return 255
    pidof systemd > /dev/null || return 255

    local DF="${srcdir}/test_probes_systemdunitproperty.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    [ -f $RF ]
    verify_results "def" $DF $RF 8
    verify_results "tst" $DF $RF 8
    rm $RF
}

test_probes_systemdunitproperty
