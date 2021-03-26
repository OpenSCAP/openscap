#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

xccdf="$srcdir/test_utils_args_xccdf_profile.xml"
r1="test_utils_args1.log"
r2="test_utils_args2.log"

$OSCAP info --verbose DEVEL --profiles "$xccdf" 1> "$r1"
$OSCAP info --verbose=DEVEL --profiles "$xccdf" 1> "$r2"

diff "$r1" "$r2"

rm -f "$r1" "$r2"
