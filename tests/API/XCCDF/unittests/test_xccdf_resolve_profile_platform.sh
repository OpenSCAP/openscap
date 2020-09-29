#!/usr/bin/env bash
. $builddir/tests/test_common.sh

########################################################################
### Test "oscap xccdf resolve" command on a Profile with platform
########################################################################

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.res.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)


echo "Stderr file = $stderr"
echo "Result file = $result"

$OSCAP xccdf resolve --output $result $srcdir/${name}.xccdf.xml > $stdout
$OSCAP xccdf validate $result >> $stdout

assert_exists 1 '//Benchmark[@resolved="1"]'

# Resolve Profile Platform
assert_exists 2 '//Profile[@id="xccdf_resolve_profile_platform"]/select'
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_platform"]/platform[@idref="cpe:/a:open-scap:oscap"]'

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
rm $result
