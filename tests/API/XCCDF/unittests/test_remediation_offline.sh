#!/bin/bash
. $builddir/tests/test_common.sh

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.res.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
root=$(mktemp -d -t ${name}.root.XXXXXX)

set_chroot_offline_test_mode "$root"

$OSCAP xccdf eval --remediate --results $result $srcdir/${name}.xccdf.xml 2> $stderr 1> $stdout || echo "Scanner returned non-zero code (OK)"
rm $result

grep "remediation in offline mode: not implemented" $stderr > /dev/null

rm -rf $stderr $stdout $root

unset_chroot_offline_test_mode
