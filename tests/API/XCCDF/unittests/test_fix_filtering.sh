#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(make_temp_file /tmp ${name}.out)
stderr=$(make_temp_file /tmp ${name}.out)

echo "Stderr file = $stderr"
echo "Result file = $result"

line1='^\W*part /var$'

$OSCAP xccdf generate fix --fix-type anaconda \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep "$line1" $result

[ "`grep -v "$line1" $result | xsed 's/\W//g'`"x == x ]

rm $result
