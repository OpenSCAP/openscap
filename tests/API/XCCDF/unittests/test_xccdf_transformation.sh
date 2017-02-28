#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.xccdf12.XXXXXX)

xsltproc --stringparam reverse_DNS com.example.www $srcdir/../../../../xsl/xccdf_1.1_to_1.2.xsl $srcdir/../../../nist/R1100/r1100-scap11-win_rhel-xccdf.xml > $result

[ -s $result ]

$OSCAP xccdf validate $result

assert_exists 1 '/Benchmark[namespace-uri()="http://checklists.nist.gov/xccdf/1.2"]'

rm $result
