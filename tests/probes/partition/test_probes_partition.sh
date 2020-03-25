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

    $OSCAP oval --verbose=DEVEL eval --results $RF $DF 2> $stderr

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    rm $stderr

    return $ret_val
}

test_probes_partition