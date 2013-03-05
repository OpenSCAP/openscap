#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
echo "Stderr file = $stderr"
rm -f test_file

$OSCAP xccdf remediate $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]

$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile002 $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ ! -f test_file ]

ret=0
$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile001 $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 2 ]
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ -f test_file ]; rm test_file

$OSCAP xccdf remediate --result-id xccdf_org.open-scap_testresult_default-profile $srcdir/${name}.xccdf.xml 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
[ -f test_file ]; rm test_file

