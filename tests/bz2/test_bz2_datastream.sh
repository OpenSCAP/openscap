#!/usr/bin/env bash
#
# Copyright 2014 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

name=$(basename $0 .sh)
dir=$(mktemp -d -t ${name}.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)
echo "Stderr file = $stderr"
sds=$dir/sds.xml
xccdf=$dir/xccdf.xml
cp $srcdir/../DS/sds_multiple_oval/*.xml $dir/
cp $srcdir/../bz2/sds.xml $sds
mv $dir/multiple-oval-xccdf.xml $xccdf

#
# Checks before DataStream compose
#
bzip2 $xccdf
[ -f "${xccdf}.bz2" ]
$OSCAP info "${xccdf}.bz2" 2> $stderr
[ -f $stderr ]; [ ! -s $stderr ]

$OSCAP xccdf validate --skip-schematron "${xccdf}.bz2" > $stderr
[ ! -s $stderr ]
bash $builddir/run ./test_bz2_memory_source "${xccdf}.bz2" | grep 'XCCDF Checklist'

#
# Compose DataStream
#
bzip2 $sds
[ -f "${sds}.bz2" ]
$OSCAP info "${sds}.bz2" 2> $stderr
[ ! -s $stderr ]

$OSCAP ds sds-validate "${sds}.bz2" > $stderr
[ ! -s $stderr ]
bash $builddir/run ./test_bz2_memory_source "${sds}.bz2" | grep 'SCAP Source Datastream'

#
# Evaluate
#
ret=0
$OSCAP xccdf eval "${xccdf}.bz2" 2> $stderr || ret=$?
[ $ret -eq 2 -o $ret -eq 0 ]
[ ! -s $stderr ]

ret=0
arf=$dir/arf.xml
$OSCAP xccdf eval --results-arf $arf "${sds}.bz2" 2> $stderr || ret=$?
[ $ret -eq 2 -o $ret -eq 0 ]
[ ! -s $stderr ]

#
# Generate report from ARF
#
bzip2 $arf
[ -f "${arf}.bz2" ]
report=$dir/report.html
$OSCAP xccdf generate report --output $report "${arf}.bz2" 2> $stderr
[ ! -s $stderr ]
[ -f $report ]
bash $builddir/run ./test_bz2_memory_source "${arf}.bz2" | grep 'ARF Result Datastream'

grep -q 'OVAL test results details' $report
rm $stderr
rm -rf $dir
