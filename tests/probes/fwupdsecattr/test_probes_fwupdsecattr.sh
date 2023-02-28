#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_fwupdsecattr {

    probecheck "fwupdsecattr" || return 255
    require_dbus "system" "org.freedesktop.fwupd" "/" || return 255

    # Check if org.freedesktop.fwupd.GetHostSecurityAttrs method
    # is supported (it might fail, e.g. in hypervisor env).
    gdbus call --system -d org.freedesktop.fwupd -o / \
        -m org.freedesktop.fwupd.GetHostSecurityAttrs >/dev/null || return 255

    local ret_val=0
    local DF="${srcdir}/test_probes_fwupdsecattr.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    return $ret_val
}

test_probes_fwupdsecattr
