#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail

kickstart=$(mktemp)

$OSCAP xccdf generate fix --fix-type kickstart --output "$kickstart" --profile common "$srcdir/test_remediation_kickstart.ds.xml"

grep -q "# Kickstart for Common hardening profile" "$kickstart"

rm -rf "$kickstart"
