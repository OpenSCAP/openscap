#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
tmpdir=$(mktemp -t -d "${name}.XXXXXX")
tpl=${srcdir}/${name}.xml.tpl
input=${tmpdir}/${name}.xml
result=${tmpdir}/${name}.results.xml
echo "Temp dir: $tmpdir"

# prepare the environment
sed "s@%PATH%@${tmpdir}@" $tpl > $input
echo "line1" > "${tmpdir}/textfile"
echo "line2" >> "${tmpdir}/textfile"
echo "line3" >> "${tmpdir}/textfile"

echo "Evaluating content."
$OSCAP oval eval --results $result $input || [ $? == 2 ]
echo "Validating results."
$OSCAP oval validate-xml --results $result
echo "Testing syschar values."
# filename
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filename:tst:1"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filename:tst:2"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filename:tst:3"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filename:tst:4"]/@result)')" == "false" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filename:tst:5"]/@result)')" == "true" ]
# filepath
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filepath:tst:1"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filepath:tst:2"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filepath:tst:3"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filepath:tst:4"]/@result)')" == "false" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:filepath:tst:5"]/@result)')" == "true" ]
echo "Testing syschar values."
# filename
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filename:obj:1"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filename:obj:2"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filename:obj:3"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filename:obj:4"]/@flag)')" == "does not exist" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filename:obj:5"]/@flag)')" == "complete" ]
#filepath
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filepath:obj:1"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filepath:obj:2"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filepath:obj:3"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filepath:obj:4"]/@flag)')" == "does not exist" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:filepath:obj:5"]/@flag)')" == "complete" ]

rm -rf $tmpdir
