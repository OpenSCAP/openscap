#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_process58_offline_mode {
    probecheck "process58" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_process58_offline_mode.xml"
    local RF="test_probes_process58_offline_mode.results.xml"
    echo "result file: $RF"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_process58.XXXXXX")

    ln -s /proc $tmpdir

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF 2> $stderr

    unset_chroot_offline_test_mode

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    rm $stderr
    rm -rf ${tmpdir}

    return $ret_val
}

test_probes_process58_offline_mode