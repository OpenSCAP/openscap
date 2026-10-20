#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_environmentvariable58_offline_mode {
    probecheck "environmentvariable58" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_environmentvariable58_offline_mode.sh.xml"
    local RF="test_probes_environmentvariable58_offline_mode.sh.results.xml"
    echo "result file: $RF"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_environmentvariable58.XXXXXX")
    mkdir -p "$tmpdir/proc/1"

    set_chroot_offline_test_mode "$tmpdir"

    OSCAP_CONTAINER_VARS=$'CONTAINER_VARIABLE=/some/value/for/container\nOTHER_CONTAINER_VAR=other' $OSCAP oval eval --results $RF $DF 2> $stderr

    unset_chroot_offline_test_mode

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"

    rm $stderr

    rm -rf ${tmpdir}

    return $ret_val
}

test_run "test_probes_environmentvariable58_offline_mode" test_probes_environmentvariable58_offline_mode
