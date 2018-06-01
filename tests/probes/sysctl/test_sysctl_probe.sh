#!/bin/bash

. ../../test_common.sh

set -e -o pipefail

function perform_test {
probecheck "sysctl" || return 255

result=`mktemp`
stderr=`mktemp`
hostname=`hostname`
$OSCAP oval eval --results $result $srcdir/test_sysctl_probe.oval.xml 2>$stderr

[ ! -s $stderr ]
assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:name[text()='kernel.hostname']"
assert_exists 1 "/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:sysctl_item/unix-sys:value[text()='$hostname']"

rm $result
rm $stderr
}

perform_test
