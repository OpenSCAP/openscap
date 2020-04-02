#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function test_probes_selinuxsecuritycontext_offline_mode {
    local VALID=$1

    # if the user doesn't have SELinux enabled we will skip the test
    selinuxenabled || return 255

    probecheck "selinuxboolean" || return 255

    local DF="${srcdir}/test_probes_selinuxsecuritycontext.xml"
    local RF="test_probes_selinuxsecuritycontext_offline_mode.results.xml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_selinuxsecuritycontext.XXXXXX")

    if [[ "${VALID}" == "true" ]]; then
        ln -s /dev $tmpdir
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

test_run "Basic selinuxsecuritycontext probe test for offline mode"                  test_probes_selinuxsecuritycontext_offline_mode "true"
test_run "Basic selinuxsecuritycontext probe test for offline mode (invalid prefix)" test_probes_selinuxsecuritycontext_offline_mode "false"
