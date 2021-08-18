#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_systemdunitproperty {
    probecheck "systemdunitproperty" || return 255
    pidof systemd > /dev/null || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_systemdunitproperty.xml"
    local RF="results.xml"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF 2>$stderr

    [ -f $RF ]
    verify_results "def" $DF $RF 8
    verify_results "tst" $DF $RF 8

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    rm $RF

    return $ret_val
}

test_run "Probe systemdunitproperty general functionality" test_probes_systemdunitproperty
