#!/usr/bin/env bash

set -o pipefail

# This test is dependent on /etc/login.defs being present, so skip this test if it doesn't exist.
[ ! -f "/etc/login.defs" ] && exit 255

stderr=`mktemp`

$OSCAP oval eval $srcdir/test_object_component_type.oval.xml 2> $stderr
ret=$?
set -e
[ $ret -eq 1 ]

grep -q "Entity [']something_bogus['] has not been found in textfilecontent_item (id: [0-9]\+) specified by object [']oval:oscap:obj:10[']." $stderr
grep -q "Expected record data type, but found string data type in subexpression entity in textfilecontent_item (id: [0-9]\+) specified by object [']oval:oscap:obj:10[']."  $stderr

rm $stderr

