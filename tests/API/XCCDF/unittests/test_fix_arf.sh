#!/bin/bash

set -e
set -o pipefail

profile="xccdf_moc.elpmaxe.www_profile_standard"
result_id="xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_standard"
bash_line1="echo this_is_ok"
bash_line2="echo fix_me_please"
ansible_template="urn:xccdf:fix:script:ansible"
ansible_task1a="\- name: ensure everything passes"
ansible_task1b="shell: /bin/true"
ansible_task2a="\- name: correct the failing case"
ansible_task2b="shell: /bin/false"

# Regex matches for script header comments, including: line of #, #\n, # Example Comment
# Avoids matching # - hosts: and required ansible header elements
regex="^$|[\#]$|^([\#][[:blank:]][^\-])"

name=$(basename $0 .sh)
results_arf=$(mktemp -t ${name}.arf.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)
script=$(mktemp -t ${name}.sh.XXXXXX)
playbook=$(mktemp -t ${name}.yml.XXXXXX)

# Create an ARF
$OSCAP xccdf eval --profile $profile --results-arf $results_arf $srcdir/$name.xccdf.xml >$stdout 2>$stderr || [ $? == 2 ]
[ -f $stdout ]; [ -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

# Generate a bash script from a profile in ARF file
$OSCAP xccdf generate fix --profile $profile --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q "$bash_line1" $script
grep -q "$bash_line2" $script

# Generate an Ansible playbook from a profile in ARF file
$OSCAP xccdf generate fix --profile $profile --template $ansible_template $results_arf | grep -Ev $regex >$playbook 2>$stderr
diff -B $playbook $srcdir/$name.playbook1.yml
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q "$ansible_task1a" $playbook
grep -q "$ansible_task1b" $playbook
grep -q "$ansible_task2a" $playbook
grep -q "$ansible_task2b" $playbook

# Generate a bash script based on scan results stored in ARF file
$OSCAP xccdf generate fix --result-id $result_id --output $script $results_arf >$stdout 2>$stderr
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q -v "$bash_line1" $script
grep -q "$bash_line2" $script

# Generate  an Ansible playbook based on scan results stored in ARF file
$OSCAP xccdf generate fix --result-id $result_id --template $ansible_template $results_arf | grep -Ev $regex >$playbook 2>$stderr
diff -B $playbook $srcdir/$name.playbook2.yml
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
grep -q -v "$ansible_task1a" $playbook
grep -q -v "$ansible_task1b" $playbook
grep -q "$ansible_task2a" $playbook
grep -q "$ansible_task2b" $playbook

rm $results_arf
rm $script
rm $playbook
