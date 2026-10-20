#!/usr/bin/env bash
set -e -o pipefail

. $builddir/tests/test_common.sh

$OSCAP info "$top_srcdir/tests/API/XCCDF/unittests/openscap_2289_regression.xml"
