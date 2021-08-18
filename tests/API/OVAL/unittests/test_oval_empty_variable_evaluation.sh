#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail

# This test is dependent on /etc/issue being present, so skip this test if it doesn't exist.
[ ! -f "/etc/issue" ] && exit 255

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "Result file: $result"

echo "Evaluating content."
$OSCAP oval eval --results $result $srcdir/${name}.xml || [ $? == 2 ]
echo "Validating results."
$OSCAP oval validate --results $result
echo "Testing results values."
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/@result)')" == "false" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/@result)')" == "error" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/@result)')" == "true" ]
echo "Testing syschar values."
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]/@flag)')" == "does not exist" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:11"]/@flag)')" == "does not exist" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:2"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:21"]/@flag)')" == "complete" ]

rm $result
