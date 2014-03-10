#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.res.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
echo "Result file = $result"

$OSCAP xccdf eval \
	--results $result $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]
cat $stdout | grep "This title is variable: No profile"; :> $stdout
$OSCAP xccdf validate-xml $result; :> $result

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 \
	--results $result $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]
cat $stdout | grep "This title is variable: The First Profile"; :> $stdout
$OSCAP xccdf validate-xml $result; :> $result

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_2 \
	--results $result $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
cat $stdout | grep "This title is variable: The Second Profile"; rm $stdout
$OSCAP xccdf validate-xml $result; rm $result
