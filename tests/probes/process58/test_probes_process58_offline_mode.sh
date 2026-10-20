#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_process58_offline_mode {
    local VALID=$1

    probecheck "process58" || return 255

    local ret_val=0;
    local DF="test_probes_process58_offline_mode.xml"
    local RF="test_probes_process58_offline_mode.results.${VALID}.xml"
    echo "result file: $RF"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF
    ${srcdir}/test_probes_process58_offline_mode.xml.sh $VALID > $DF

    tmpdir=$(mktemp -t -d "test_offline_mode_process58.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /proc $tmpdir
    fi

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF 2>$stderr

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

test_run "Ensure that probe handles \$OSCAP_CHROOT"      test_probes_process58_offline_mode "true"
test_run "Ensure that probe handles empty \$SCAP_CHROOT" test_probes_process58_offline_mode "false"
