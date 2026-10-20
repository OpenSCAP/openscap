#!/usr/bin/env bash

# Copyright 2014--2016 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_systemdunitdependency_offline_mode {
    local VALID=$1

    probecheck "systemdunitdependency" || return 255
    pidof systemd > /dev/null || return 255

    local ret_val=0;
    local DF="test_probes_systemdunitdependency_offline_mode.xml"
    local RF="test_probes_systemdunitdependency_offline_mode.results.${VALID}.xml"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF
    ${srcdir}/test_probes_systemdunitdependency_offline_mode.xml.sh $VALID > $DF

    tmpdir=$(mktemp -t -d "test_offline_mode_systemdunitdependency.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /run $tmpdir
    fi

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF 2>$stderr

    unset_chroot_offline_test_mode

    [ -f $RF ]

    result="$RF"

    if [[ "${VALID}" == "true" ]]; then
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:0:tst:1"]'
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:0:tst:2"]'
    else
        assert_exists 1 '/oval_results/results/system/tests/test[@result="unknown"][@test_id="oval:0:tst:1"]'
        assert_exists 1 '/oval_results/results/system/tests/test[@result="unknown"][@test_id="oval:0:tst:2"]'
    fi

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    rm $RF
    rm -rf ${tmpdir}

    return $ret_val
}

test_run "Probe systemdunitdependency offline functionality"                  test_probes_systemdunitdependency_offline_mode "true"
test_run "Probe systemdunitdependency offline functionality (invalid prefix)" test_probes_systemdunitdependency_offline_mode "false"
