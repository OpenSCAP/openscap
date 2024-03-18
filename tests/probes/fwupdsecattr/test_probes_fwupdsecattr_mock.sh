#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_fwupdsecattr {

    probecheck "fwupdsecattr" || return 255

    local ret_val=0
    local DF="${srcdir}/test_probes_fwupdsecattr_mock.xml"
    local RF="results.xml"
    local DBUS_MOCK_NAME="org.freedesktop.fwupd"
    local stderr=$(mktemp test_probes_fwupdsecattr_mock.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    init_dbus_mock $DBUS_MOCK_NAME
    $OSCAP oval eval --results $RF $DF 2>$stderr || true
    clean_dbus_mock $DBUS_MOCK_NAME

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    grep -iq "HSI key not found: org.fwupd.hsi.Kernel.InvalidOrNonExisting" $stderr || {
        ret_val=2
        echo "Expected warning 'HSI key not found: org.fwupd.hsi.Kernel.InvalidOrNonExisting' is missing!"
    }

    grep -iq "Unknown/invalid FwupdSecurityAttrResult value: 200" $stderr || {
        ret_val=2
        echo "Expected warning 'Unknown/invalid FwupdSecurityAttrResult value: 200' is missing!"
    }

    return $ret_val
}

test_probes_fwupdsecattr
