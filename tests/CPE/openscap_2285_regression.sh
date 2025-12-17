#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

# oscap is expcted to fail here.
# Turn off pipefail, run the cmd, the return code and return to safety. 
set +e +o pipefail
$OSCAP info "${top_srcdir}/tests/CPE/openscap_2285_regression.xml" 
rc=$?
set -e -o pipefail


if [ "$rc" -eq 139 ]; then
    echo "oscap seg-faulted (exit status 139)"
    exit 1
fi

if [ "$rc" -eq 0 ]; then
	echo "oscap did not error as expected"
	exit 1
fi
