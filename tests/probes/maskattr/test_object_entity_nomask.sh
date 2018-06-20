#!/bin/bash

. ../../test_common.sh

set -e -o pipefail

function perform_test {
probecheck "file" || return 255

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "Result file: $result"

echo "Evaluating content."
$OSCAP oval eval --results $result $srcdir/${name}.xml || [ $? == 2 ]

echo "Validating results."
$OSCAP oval validate-xml --results $result

echo "Testing results values."
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/@result)')" == "true" ]

echo "Testing syschar values."
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'count(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]/reference)')" == "1" ]

[ "$($XPATH $result 'boolean(/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item/unix-sys:filepath/@mask)')" == "0" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item/unix-sys:filepath)')" == "/etc/passwd" ]

rm $result
}

perform_test
