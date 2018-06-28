#!/bin/bash

set -e
set -o pipefail

. $builddir/tests/test_common.sh

function perform_test {
# if xsltproc is not installed we will skip the test
require xsltproc || return 255

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.xccdf12.XXXXXX)

xsltproc --stringparam reverse_DNS com.example.www $srcdir/../../../../xsl/xccdf_1.1_to_1.2.xsl $srcdir/../../../nist/R1100/r1100-scap11-win_rhel-xccdf.xml > $result

[ -s $result ]

$OSCAP xccdf validate $result

assert_exists 1 '//*[namespace::*="http://checklists.nist.gov/xccdf/1.2"]'

rm $result
}

perform_test
