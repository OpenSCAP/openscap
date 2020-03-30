#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_partition {
    local VALID=$1

    probecheck "partition" || return 255

    local DF="${srcdir}/test_probes_partition.xml"
    local RF="test_probes_partition_offline_mode.results.xml"
    echo "result file: $RF"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_partition.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /proc $tmpdir
    fi

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode

    [ -f $RF ]

    result="$RF"

    if [[ "${VALID}" == "true" ]]; then
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:1:tst:1"]'
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
    else
        assert_exists 1 '/oval_results/results/system/tests/test[@result="false"][@test_id="oval:1:tst:1"]'
    fi

    rm -rf ${tmpdir}
}

test_run "Probe partition simple offline test (/proc mount point)"                  test_probes_partition "true"
test_run "Probe partition simple offline test (/proc mount point) (invalid prefix)" test_probes_partition "false"