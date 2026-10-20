#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh
probecheck "process58" || exit 255

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
echo "result file: $result"
stderr=$(mktemp ${name}.err.XXXXXX)
echo "stderr file: $stderr"

specarg="--report $(echo -ne "\e\n\E[1;33m")\\\n\e"
specarg_escaped="--report . .[1;33m\\\\n\\e"

pcmdl="oval eval --results $result $srcdir/$name.oval.xml"

$OSCAP oval eval --results $result $srcdir/$name.oval.xml $specarg 2> $stderr
[ ! -s $stderr ]

rm $stderr

[ -s $result ]
assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:process58_item/unix-sys:command_line[contains(., '$pcmdl ${specarg_escaped}')]"

rm $result
