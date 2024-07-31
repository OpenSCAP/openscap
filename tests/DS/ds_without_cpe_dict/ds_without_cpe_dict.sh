#!/usr/bin/env bash

. $builddir/tests/test_common.sh
set -e -o pipefail

stdout=$(mktemp)
stderr=$(mktemp)
$OSCAP xccdf eval --progress $srcdir/ds_without_cpe_dict.xml > $stdout 2> $stderr
[ -e $stderr ]
grep -q "xccdf_moc.elpmaxe.www_rule_1:pass" $stdout
! grep -q "xccdf_moc.elpmaxe.www_rule_1:notapplicable" $stdout
! grep -q "Can't import OVAL definition model 'cpe-oval.xml' for CPE applicability checking" $stderr
rm -rf $stdout $stderr
