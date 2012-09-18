#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "Result file: $result"

echo "Evaluating content."
$OSCAP oval eval --results $result $srcdir/${name}.xml || [ $? == 2 ]
echo "Validating results."
$OSCAP oval validate-xml --results $result
echo "Testing results values."
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/@result)')" == "false" ]
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/@result)')" == "true" ]
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/@result)')" == "error" ]
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/@result)')" == "true" ]
echo "Testing syschar values."
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]/@flag)')" == "does not exist" ]
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:11"]/@flag)')" == "does not exist" ]
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:2"]/@flag)')" == "complete" ]
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:21"]/@flag)')" == "complete" ]

rm -rf $result
