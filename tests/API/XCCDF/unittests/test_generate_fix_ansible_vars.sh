#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

profile="xccdf_com.example.www_profile_test_ansible_vars"
profile_tailored="xccdf_com.example.www_profile_test_ansible_vars_tailored"
ds="test_generate_fix_ansible_vars_ds.xml"
tailoring_file="test_generate_fix_ansible_vars_ds-tailoring.xml"
golden="test_generate_fix_ansible_vars_golden.yml"
golden_altered="test_generate_fix_ansible_vars_golden_altered.yml"
golden_tailored="test_generate_fix_ansible_vars_golden_tailoring.yml"
var="www_value_val1"

name=$(basename $0 .sh)
playbook=$(make_temp_file /tmp ${name}.yml)
out=$(make_temp_file /tmp ${name}.out)

$OSCAP xccdf generate fix --profile $profile --fix-type ansible \
	$srcdir/$ds >$playbook 2>$out
[ -f $out ]; [ ! -s $out ]; :> $out
[ -f $playbook ]; [ -s $playbook ]
# Removes comment and blank lines from the generated playbook.
xsed -i '/#.*/d' $playbook
xsed -i '/^[ \t]*$/d' $playbook

# Compares golden playbook with generated playbook to ensure that Ansible
# variables were generated correctly. Both playbooks must be the same.
diff -u $srcdir/$golden $playbook >$out
[ -f $out ]; [ ! -s $out ]; :> $out

# Compares value of Ansible variable $var from the golden altered playbook with
# the $var from the generated playbook. Values of the Ansible variables $var
# must differ (altered golden playbook has different value set).
generated_var=$(grep "$var:" $playbook | xsed "s|.*$var:[^0-9]*||")
golden_altered_var=$(grep "$var:" $srcdir/$golden_altered | xsed "s|.*$var:[^0-9]*||")

[ "$generated_var" != "$golden_altered_var" ]

# Generates Ansible playbook using tailoring file.
$OSCAP xccdf generate fix --fix-type ansible \
	--profile $profile_tailored --tailoring-file $srcdir/$tailoring_file \
	$srcdir/$ds >$playbook 2>$out
[ -f $out ]; [ ! -s $out ]; :> $out
[ -f $playbook ]; [ -s $playbook ]
# Removes comment and blank lines from the generated playbook.
xsed -i '/#.*/d' $playbook
xsed -i '/^[ \t]*$/d' $playbook

# Compares golden tailored playbook with generated playbook to ensure that only
# tailored Ansible variables are generated in the playbook. Both playbooks must
# be the same.
diff -u $srcdir/$golden_tailored $playbook >$out
[ -f $out ]; [ ! -s $out ]; :> $out


rm $playbook $out
