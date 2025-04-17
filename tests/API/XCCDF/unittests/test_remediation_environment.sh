#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)

rm -f remediation.env

CANARY_EXPORTED="CANARY_EXPORTED_VALUE"
export CANARY_EXPORTED
CANARY_PROCESS="CANARY_PROCESS_VALUE" $OSCAP xccdf eval --remediate $srcdir/${name}.xccdf.xml || true

grep -q "${PATH}" remediation.env || die "PATH not found"
grep -q "CANARY_EXPORTED_VALUE" remediation.env || die "CANARY_EXPORTED_VALUE not found"
grep -q "CANARY_PROCESS_VALUE" remediation.env || die "CANARY_PROCESS_VALUE not found"

rm -f remediation.env
rm $result
