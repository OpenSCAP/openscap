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
    systemctl > /dev/null 2>&1 || return 255

    local ret_val=0;
    local DF="test_probes_systemdunitdependency.xml"
    local RF="test_probes_systemdunitdependency.results.xml"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF
    ${srcdir}/test_probes_systemdunitdependency.xml.sh "true" > $DF 2>$stderr

    $OSCAP oval eval --results $RF $DF

    [ -f $RF ]
    verify_results "def" $DF $RF 3
    verify_results "tst" $DF $RF 3

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    rm $RF

    return $ret_val
}

test_run "Probe systemdunitdependency general functionality" test_probes_systemdunitdependency
