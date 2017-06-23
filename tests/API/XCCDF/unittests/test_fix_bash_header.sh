#!/bin/bash

set -x
set -e
set -o pipefail

profile="xccdf_moc.elpmaxe.www_profile_standard"
result_id="xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_standard"
title="Standard System Security Profile"
description="This profile contains rules to ensure standard security baseline of your system."
benchmark="xccdf_moc.elpmaxe.www_benchmark_test"
version="1.0"
xccdf_version="1.2

name=$(basename $0 .sh)
results_arf=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
script=$(mktemp -t ${name}.sh.XXXXXX)

bash_header1="# Bash remediation role for $profile"
bash_header2="# Profile Title:  $title"
bash_header3="# Profile Description:  $description"
bash_header4="# Benchmark ID:  $benchmark"
bash_header5="# Benchmark Version:  $version"
bash_header6="# XCCDF Version:  $xccdf_version"

# Create an ARF
$OSCAP xccdf eval --profile $profile --results-arf $results_arf $srcdir/$name.xccdf.xml >$stdout 2>$stderr || [ $? == 2 ]
[ -f $stdout ]; [ -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

# Generate a bash script from a profile in ARF file
$OSCAP xccdf generate fix --profile $profile --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep  "$bash_header1" $script
grep  "$bash_header2" $script
grep  "$bash_header3" $script
grep  "$bash_header4" $script
grep  "$bash_header5" $script
grep  "$bash_header6" $script

# Generate a bash script based on scan resu
rm -f test_filelts stored in ARF file
$OSCAP xccdf generate fix --result-id $result_id --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

rm $results_arf
rm $script

