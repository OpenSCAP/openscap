#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_systemdunitdependency_offline_mode {
    probecheck "systemdunitdependency" || return 255
    pidof systemd > /dev/null || return 255

    local DF="${srcdir}/test_probes_systemdunitdependency.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_systemdunitdependency.XXXXXX")

    ln -s /run $tmpdir

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode

    [ -f $RF ]
    verify_results "def" $DF $RF 3
    verify_results "tst" $DF $RF 3
    rm $RF
    rm -rf ${tmpdir}
}

test_probes_systemdunitdependency_offline_mode
