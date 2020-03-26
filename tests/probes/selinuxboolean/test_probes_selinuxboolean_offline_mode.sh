#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function test_probes_selinuxboolean_offline_mode {
    # if the user doesn't have SELinux enabled we will skip the test
    selinuxenabled || return 255

    probecheck "selinuxboolean" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_selinuxboolean_offline_mode.xml"
    local RF="test_probes_selinuxboolean_offline_mode.results.xml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_selinuxboolean.XXXXXX")

    ln -s /sys $tmpdir

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    rm -rf ${tmpdir}

    return $ret_val
}

test_probes_selinuxboolean_offline_mode
