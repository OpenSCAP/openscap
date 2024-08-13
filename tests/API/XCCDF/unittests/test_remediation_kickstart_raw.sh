#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail

function test_raw {
    kickstart=$(mktemp)
    stderr=$(mktemp)
    expected_modified=$(mktemp)

    sed "s;TEST_DATA_STREAM_PATH;$srcdir/test_remediation_kickstart.ds.xml;" "$srcdir/test_remediation_kickstart_expected_raw.cfg" > "$expected_modified"

    $OSCAP xccdf generate fix --fix-type kickstart --raw --output "$kickstart" --profile common "$srcdir/test_remediation_kickstart.ds.xml"

    diff -u "$expected_modified" "$kickstart"

    rm -rf "$kickstart"
    rm -rf "$stderr"
    rm -rf "$expected_modified"
    rm -rf "$kickstart_modified"
}

test_raw
