#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

$OSCAP info "${top_srcdir}/tests/oval_details/openscap_2288_regression.xml"
