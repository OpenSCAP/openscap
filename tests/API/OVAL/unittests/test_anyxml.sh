#! /bin/bash

result=`mktemp`
xpath=`which xpath`

set -e
set -o pipefail

$OSCAP oval analyse --results $result $srcdir/anyxmloval.xml $srcdir/anyxmlsyschar.xml

[ $($xpath $result 'count(/oval_results/oval_definitions/generator/ai:assets)') == "1" ]
[ $($xpath $result 'count(/oval_results/oval_definitions/definitions/definition/metadata/errata:advisory)') == "1" ]
[ $($xpath $result 'count(/oval_results/results/system/oval_system_characteristics/system_info/ai:assets)') == "1" ]

rm -rf $result

