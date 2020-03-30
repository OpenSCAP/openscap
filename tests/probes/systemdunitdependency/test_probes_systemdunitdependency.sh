#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_systemdunitdependency {
    probecheck "systemdunitdependency" || return 255
    pidof systemd > /dev/null || return 255

    local DF="test_probes_systemdunitdependency.xml"
    local RF="test_probes_systemdunitdependency.results.xml"

    [ -f $RF ] && rm -f $RF
    ${srcdir}/test_probes_systemdunitdependency.xml.sh "true" > $DF

    $OSCAP oval eval --results $RF $DF

    [ -f $RF ]
    verify_results "def" $DF $RF 3
    verify_results "tst" $DF $RF 3
    rm $RF
}

test_run "Probe systemdunitdependency general functionality" test_probes_systemdunitdependency
