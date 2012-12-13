#!/bin/bash

set -e
set -o pipefail

result=`mktemp`
stderr=`mktemp`

$OSCAP xccdf eval --results $result $srcdir/test_xccdf_check_content_ref_without_name_attr.xccdf.xml 2> $stderr || [ $? == 2 ]

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

[ $($XPATH $result 'count(/Benchmark/status[not(@date)])') == "1" ]
[ $($XPATH $result 'count(//rule-result)') == "1" ]
[ $($XPATH $result '//rule-result[@idref="def-20120006"]/result/text()') == "fail" ]
[ $($XPATH $result 'count(//rule-result/check/check-content-ref)') == "1" ]
[ $($XPATH $result 'count(//rule-result/check/check-content-ref[not(@name)])') == "1" ]
[ $($XPATH $result 'count(//check[@multi-check])') == "0" ]
[ $($XPATH $result '/Benchmark/TestResult/score[@system="urn:xccdf:scoring:default"]/text()') == "0.000000" ]
rm $result
