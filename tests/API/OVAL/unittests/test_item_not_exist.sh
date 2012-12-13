#! /bin/bash

result=`mktemp`
xpath="$XPATH"

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/item_not_exist.xml


[ $($xpath $result 'count(/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:org.mitre.oval.test:obj:512"][@flag="does not exist"])') == "1" ]
[ $($xpath $result 'count(/oval_results/results/system/oval_system_characteristics/system_data)') == "0" ]
rm $result

