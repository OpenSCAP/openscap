#!/bin/bash

set -x
set -e
set -o pipefail

ansible_template="urn:xccdf:fix:script:ansible"
profile="xccdf_moc.elpmaxe.www_profile_standard"
result_id="xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_standard"
title="Standard System Security Profile"
description="This profile contains rules to ensure standard security baseline of your system."
benchmark="xccdf_moc.elpmaxe.www_benchmark_test"
version="1.0"
xccdf_version="1.2"

name=$(basename $0 .sh)
results_arf=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
script=$(mktemp -t ${name}.sh.XXXXXX)
playbook=$(mktemp -t ${name}.yml.XXXXXX)

profile_header1="# Bash remediation role for profile $profile"
profile_header2="# Profile Title:  $title"
profile_header3="# Profile Description:  $description"
profile_header4="# Benchmark ID:  $benchmark"
profile_header5="# Benchmark Version:  $version"
profile_header6="# XCCDF Version:  $xccdf_version"

result_header1="# Bash remediation role for the results of evaluation of profile $profile"

# Create an ARF
$OSCAP xccdf eval --profile $profile --results-arf $results_arf $srcdir/$name.xccdf.xml >$stdout 2>$stderr || [ $? == 2 ]
[ -f $stdout ]; [ -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

# Generate a bash script from an OpenSCAP profile 
$OSCAP xccdf generate fix --profile $profile --output $script $srcdir/$name.xccdf.xml >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep "$profile_header1" $script
grep "$profile_header2" $script
grep "$profile_header3" $script
grep "$profile_header4" $script
grep "$profile_header5" $script
grep "$profile_header6" $script

# Generate a bash script based on scan results
$OSCAP xccdf generate fix --result-id $result_id --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep "$result_header1" $script


# Generate an Ansible playbook from an OpenSCAP profile
$OSCAP xccdf generate fix --profile $profile --template $ansible_template --output $playbook $srcdir/$name.xccdf.xml >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep "$profile_header1" $playbook
grep "$profile_header2" $playbook
grep "$profile_header3" $playbook
grep "$profile_header4" $playbook
grep "$profile_header5" $playbook
grep "$profile_header6" $playbook

# Generate  an Ansible playbook based on scan results stored in ARF file
$OSCAP xccdf generate fix --result-id $result_id --template $ansible_template --output $playbook $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep "$result_header1" $playbook

rm $results_arf
rm $script
rm $playbook
