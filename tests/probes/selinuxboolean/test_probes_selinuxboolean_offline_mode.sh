#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_probes_selinuxboolean_offline_mode {
    local VALID=$1

    # if the user doesn't have SELinux enabled we will skip the test
    selinuxenabled || return 255

    probecheck "selinuxboolean" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_selinuxboolean_offline_mode.xml"
    local RF="test_probes_selinuxboolean_offline_mode.results.xml"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_selinuxboolean.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /sys $tmpdir
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

test_run "Basic selinuxboolean probe test for offline mode"                  test_probes_selinuxboolean_offline_mode "true"
test_run "Basic selinuxboolean probe test for offline mode (invalid prefix)" test_probes_selinuxboolean_offline_mode "false"
