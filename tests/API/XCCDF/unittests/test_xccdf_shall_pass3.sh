#!/bin/bash
. $builddir/tests/test_common.sh

set -e -o pipefail

./test_xccdf_shall_pass $srcdir/test_xccdf_overlaping_IDs.xccdf.xml

