#! /bin/bash

result=`mktemp`
xpath="$XPATH"

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/deprecated_def.xml


[ $($xpath $result 'count(/oval_results/results/system/definitions/definition[@definition_id="oval:org.mitre.oval.test:def:283"][@result="true"])') == "1" ]
[ $($xpath $result 'count(/oval_results/results/system/definitions/definition[@definition_id="oval:org.mitre.oval.test:def:282"][@result="not evaluated"])') == "1" ] && rm $result

