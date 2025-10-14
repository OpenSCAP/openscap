#!/usr/bin/env bash
set -e -o pipefail

. $builddir/tests/test_common.sh

stdout=$(mktemp)
stderr=$(mktemp)
$OSCAP xccdf eval --show-rule-details $srcdir/ds.xml > $stdout 2> $stderr || ret=$?
[ $ret = 2 ]
[ ! -s $stderr ]
[ -s $report ]
grep -q "This is a rule description" $stdout
grep -q "This is a rule rationale" $stdout
grep -q "This is a rule warning" $stdout
rm -f $stdout
rm -f $stderr
