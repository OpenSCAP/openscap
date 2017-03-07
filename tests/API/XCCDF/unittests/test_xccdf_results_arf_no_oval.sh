#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
resultArf=$(mktemp -t ${name}.arf.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

$OSCAP xccdf eval --results $result --results-arf $resultArf $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]

$OSCAP xccdf validate-xml $result
$OSCAP ds rds-validate $resultArf

$OSCAP info $resultArf > $stdout 2> $stderr
grep "^Asset: asset0" $stdout
grep "^\s*ARF report: xccdf1$" $stdout
grep "^\s*Report request: collection1$" $stdout
grep "^\s*Result ID: xccdf_org.open-scap_testresult_default-profile$" $stdout
grep "^\s*Source benchmark: .*test_xccdf_results_arf_no_oval.xccdf.xml$" $stdout
grep "^\s*Source profile: (default)$" $stdout
grep "^\s*Evaluation started: .*$" $stdout
grep "^\s*Evaluation finished: .*$" $stdout
grep "^\s*Platform CPEs:$" $stdout
grep "^\s*(none)$" $stdout
[ -f $stdout ]; [ -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr


rm $result $resultArf
