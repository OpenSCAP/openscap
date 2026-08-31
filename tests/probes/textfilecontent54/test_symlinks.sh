#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail

name=$(basename $0 .sh)
tmpdir=$(make_temp_dir /tmp ${name})
tpl=${srcdir}/${name}.xml.tpl
input=${tmpdir}/${name}.xml
result=${tmpdir}/${name}.results.xml
echo "Temp dir: $tmpdir"

# prepare the environment
sed "s@%PATH%@${tmpdir}@" $tpl > $input
ln -s /etc/hosts ${tmpdir}/sl1
ln -s /nonexistent ${tmpdir}/sl2
ln -s ${tmpdir} ${tmpdir}/sl3
mkdir -p ${tmpdir}/blocked
echo "blocked_content" > ${tmpdir}/blocked/secret
ln -s ${tmpdir}/blocked/secret ${tmpdir}/sl4
export OSCAP_PROBE_IGNORE_PATHS="${tmpdir}/blocked"

echo "Evaluating content."
$OSCAP oval eval --results $result $input || [ $? == 2 ]
echo "Validating results."
$OSCAP oval validate --results $result
echo "Testing syschar values."
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/@result)')" == "false" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/@result)')" == "false" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/@result)')" == "true" ]
[ "$($XPATH $result 'string(/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/@result)')" == "false" ]
echo "Testing syschar values."
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:2"]/@flag)')" == "does not exist" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:3"]/@flag)')" == "does not exist" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:4"]/@flag)')" == "complete" ]
[ "$($XPATH $result 'string(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:5"]/@flag)')" == "does not exist" ]

rm -rf $tmpdir
