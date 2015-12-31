#!/bin/bash

stderr=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval $srcdir/test_object_component_type.oval.xml 2> $stderr || ret=$?
[ $ret -eq 1 ]

stderr_line_count=`cat $stderr | wc -l`
[ $stderr_line_count -eq 2 ]
grep -q "Entity [']something_bogus['] has not been found in textfilecontent_item (id: [0-9]\+) specified by object [']oval:oscap:obj:10[']." $stderr
grep -q "Expected record data type, but found string data type in subexpression entity in textfilecontent_item (id: [0-9]\+) specified by object [']oval:oscap:obj:10[']."  $stderr

rm $stderr

