#!/bin/bash

set -e
set -o pipefail

profile="xccdf_moc.elpmaxe.www_profile_standard"
result_id="xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_standard"
bash_line1="echo this_is_ok"
bash_line2="echo fix_me_please"
ansible_template="urn:xccdf:fix:script:ansible"
ansible_line1="this_is_ok"
ansible_line2="fix_me_please"

name=$(basename $0 .sh)
results_arf=$(mktemp -t ${name}.arf.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)
script=$(mktemp -t ${name}.sh.XXXXXX)
playbook=$(mktemp -t ${name}.yml.XXXXXX)

# Create an ARF
$OSCAP xccdf eval --profile $profile --results-arf $results_arf $name.xccdf.xml >$stdout 2>$stderr || [ $? == 2 ]
[ -f $stdout ]; [ -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

# Generate a bash script from a profile in ARF file
$OSCAP xccdf generate fix --profile $profile --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q "$bash_line1" $script
grep -q "$bash_line2" $script

# Generate an Ansible playbook from a profile in ARF file
$OSCAP xccdf generate fix --profile $profile --template $ansible_template --output $playbook $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q "$ansible_line1" $playbook
grep -q "$ansible_line2" $playbook

# Generate a bash script based on scan results stored in ARF file
$OSCAP xccdf generate fix --result-id $result_id --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q "$bash_line2" $script

# Generate  an Ansible playbook based on scan results stored in ARF file
$OSCAP xccdf generate fix --result-id $result_id --template $ansible_template --output $playbook $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q "$ansible_line2" $playbook

rm $results_arf
rm $script
rm $playbook
