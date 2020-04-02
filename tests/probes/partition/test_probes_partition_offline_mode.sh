#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_partition {
    local VALID=$1

    probecheck "partition" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_partition.xml"
    local RF="test_probes_partition_offline_mode.results.xml"
    echo "result file: $RF"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_partition.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /proc $tmpdir
    fi

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF 2>$stderr

    unset_chroot_offline_test_mode

    [ -f $RF ]

    result="$RF"

    if [[ "${VALID}" == "true" ]]; then
        assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:1:tst:1"]'
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
    else
        assert_exists 1 '/oval_results/results/system/tests/test[@result="false"][@test_id="oval:1:tst:1"]'
    fi

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    rm -rf ${tmpdir}

    return $ret_val
}

test_run "Probe partition simple offline test (/proc mount point)"                  test_probes_partition "true"
test_run "Probe partition simple offline test (/proc mount point) (invalid prefix)" test_probes_partition "false"