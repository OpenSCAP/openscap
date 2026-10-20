#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh
probecheck "process58" || exit 255

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

ptty=`ps -p 1 --no-headers -o tty`

$OSCAP oval eval --results $result $srcdir/$name.oval.xml 2> $stderr
[ ! -s $stderr ]

rm $stderr

[ -s $result ]
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:tty[text()="'$ptty'"]'

rm $result
