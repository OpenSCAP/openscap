#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.res.XXXXXX)
report=$(mktemp -t ${name}.res.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
echo "Result file = $result"
echo "Report file = $report"

$OSCAP xccdf eval \
	--results $result --report $report $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]
cat $stdout | grep "This title is variable: No profile"; :> $stdout
$OSCAP xccdf validate $result; :> $result
grep 'This description is substituted according to the selected policy:.*No profile' $report
grep 'This title is variable:.*No profile' $report
grep 'sub ' $report || x=1; [ "x$x" == "x1" ]; unset x; :> $report
$OSCAP xccdf generate guide --output $report $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
grep 'This title is variable:.*No profile' $report
grep 'sub ' $report || x=1; [ "x$x" == "x1" ]; unset x; :> $report


$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_1 \
	--results $result --report $report $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]
cat $stdout | grep "This title is variable: The First Profile"; :> $stdout
$OSCAP xccdf validate $result; :> $result
grep 'This description is substituted according to the selected policy:.*The First Profile' $report
grep 'This title is variable:.*The First Profile' $report
grep 'sub ' $report || x=1; [ "x$x" == "x1" ]; unset x; :> $report
$OSCAP xccdf generate guide --profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $report $srcdir/${name}.xccdf.xml
grep 'This title is variable:.*The First Profile' $report
grep 'sub ' $report || x=1; [ "x$x" == "x1" ]; unset x; :> $report

$OSCAP xccdf eval --profile xccdf_moc.elpmaxe.www_profile_2 \
	--results $result --report $report $srcdir/${name}.xccdf.xml > $stdout 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]
cat $stdout | grep "This title is variable: The Second Profile"; rm $stdout
$OSCAP xccdf validate $result; rm $result
grep 'This description is substituted according to the selected policy:.*The Second Profile' $report
grep 'This title is variable:.*The Second Profile' $report
grep 'sub ' $report || x=1; [ "x$x" == "x1" ]; unset x; rm $report
$OSCAP xccdf generate guide --profile xccdf_moc.elpmaxe.www_profile_2 \
	--output $report $srcdir/${name}.xccdf.xml
grep 'This title is variable:.*The Second Profile' $report
grep 'sub ' $report || x=1; [ "x$x" == "x1" ]; unset x; :> $report
