#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

profile="xccdf_moc.elpmaxe.www_profile_standard"

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.err.XXXXXX)
playbook=$(mktemp -t ${name}.yml.XXXXXX)
playbook_without_header=$(mktemp -t ${name}.yml.XXXXXX)

# Generate an Ansible playbook from a profile in SDS file
$OSCAP xccdf generate fix --profile $profile --fix-type ansible "$srcdir/$name.xccdf.xml"  >$playbook 2>$stderr
sed '/^#/d' $playbook > $playbook_without_header
diff -u $playbook_without_header $srcdir/$name.playbook.yml
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

rm $playbook
rm $playbook_without_header
