#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_systemdunitproperty_offline_mode {
    local VALID=$1

    probecheck "systemdunitproperty" || return 255
    pidof systemd > /dev/null || return 255

    local DF="${srcdir}/test_probes_systemdunitproperty.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_systemdunitproperty.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /run $tmpdir
    fi

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode

    [ -f $RF ]

    result="$RF"

    if [[ "${VALID}" == "true" ]]; then
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:0:tst:1"]'
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:0:tst:2"]'
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:0:tst:8"]'
        verify_results "def" $DF $RF 8
        verify_results "tst" $DF $RF 8
    else
        assert_exists 1 '/oval_results/results/system/tests/test[@result="unknown"][@test_id="oval:0:tst:1"]'
        assert_exists 1 '/oval_results/results/system/tests/test[@result="unknown"][@test_id="oval:0:tst:2"]'
        assert_exists 1 '/oval_results/results/system/tests/test[@result="unknown"][@test_id="oval:0:tst:8"]'
    fi

    rm $RF
    rm -rf ${tmpdir}
}

test_run "Probe systemdunitproperty offline functionality"                  test_probes_systemdunitproperty_offline_mode "true"
test_run "Probe systemdunitproperty offline functionality (invalid prefix)" test_probes_systemdunitproperty_offline_mode "false"
