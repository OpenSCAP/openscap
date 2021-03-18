#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

report=$(mktemp)
stdout=$(mktemp)
stderr=$(mktemp)
$OSCAP xccdf eval --report $report $srcdir/ds.xml > $stdout 2> $stderr || ret=$?
[ $ret = 2 ]
[ ! -s $stderr ]
[ -f $report ]
[ -s $report ]
grep -q "Title\s*Simple rule" $stdout
grep -q "Result\s*fail" $stdout
grep -q "OpenSCAP Evaluation Report" $report
grep -q "oval:x:var:1.*42" $report
rm -f $report
rm -f $stdout
rm -f $stderr
