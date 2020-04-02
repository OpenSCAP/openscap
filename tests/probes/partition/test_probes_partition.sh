#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_partition {
    probecheck "partition" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_partition.xml"
    local RF="test_probes_partition.results.xml"
    echo "result file: $RF"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF 2>$stderr

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    return $ret_val
}

test_run "Probe partition simple test (/proc mount point)" test_probes_partition
