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
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/@result)' 2>/dev/null)" == "false" ]
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/@result)' 2>/dev/null)" == "true" ]
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/@result)' 2>/dev/null)" == "error" ]
[ "$(xpath $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/@result)' 2>/dev/null)" == "true" ]
echo "Testing syschar values."
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]/@flag)' 2>/dev/null)" == "does not exist" ]
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:11"]/@flag)' 2>/dev/null)" == "does not exist" ]
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:2"]/@flag)' 2>/dev/null)" == "complete" ]
[ "$(xpath $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:21"]/@flag)' 2>/dev/null)" == "complete" ]

rm -rf $result
