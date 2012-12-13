#! /bin/bash

result=`mktemp`
xpath="$XPATH"

$OSCAP oval eval --results $result $srcdir/applicability_check.xml

set -e
set -o pipefail

[ $($xpath $result 'count(/oval_results/oval_definitions/definitions/definition[@id="oval:gov.irs.rhel5:def:1"]/criteria[@applicability_check="true"])') == "1" ]
[ $($xpath $result 'count(/oval_results/oval_definitions/definitions/definition[@id="oval:gov.irs.rhel5:def:1"]/criteria/criterion[@applicability_check="true"])') == "1" ]
[ $($xpath $result 'count(/oval_results/oval_definitions/definitions/definition[@id="oval:gov.irs.rhel5:def:1"]/criteria/extend_definition[@applicability_check="true"])') == "1" ]
[ $($xpath $result 'count(/oval_results/results/system/definitions/definition[@definition_id="oval:gov.irs.rhel5:def:1"]/criteria[@applicability_check="true"])') == "1" ]
[ $($xpath $result 'count(/oval_results/results/system/definitions/definition[@definition_id="oval:gov.irs.rhel5:def:1"]/criteria/criterion[@applicability_check="true"])') == "1" ]
[ $($xpath $result 'count(/oval_results/results/system/definitions/definition[@definition_id="oval:gov.irs.rhel5:def:1"]/criteria/extend_definition[@applicability_check="true"])') == "1" ]
rm $result

