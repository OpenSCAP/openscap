#!/usr/bin/env bash

# This is regression test of RHBZ #2008922

set -e -o pipefail

. $builddir/tests/test_common.sh
probecheck "process58" || exit 255

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
stderr=$(mktemp ${name}.err.XXXXXX)

root=$(mktemp -d)

# create an empty /proc in the offline file system dir
mkdir -p "$root/proc"

export OSCAP_PROBE_ROOT="$root"
$OSCAP oval eval --results $result $srcdir/capability.oval.xml 2> $stderr

[ $? -eq 0 ]
grep -q "^W: oscap:\s\+No data about processes could be read from '$root/proc'." "$stderr"
grep -q "OpenSCAP Error: Probe at sd=1 (process58) reported an error: Operation not permitted" "$stderr" && false
grep -q "W: oscap:\s\+Can't receive message: 125, Operation canceled." "$stderr" && false

[ -s "$result" ]
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="false"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@flag="does not exist"]'

rm "$stderr"
rm "$result"
rm -r "$root"
