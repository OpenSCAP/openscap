#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

probecheck "file" || exit 255
which strace || exit 255

function check_strace_output {
	strace_log="$1"
	grep -q "/tmp/numbers/1" $strace_log && return 1
	grep -q "/tmp/numbers/1/2" $strace_log && return 1
	grep -q "/tmp/numbers/1/2/3" $strace_log && return 1
	grep -q "/tmp/numbers/1/2/3/4" $strace_log && return 1
	grep -q "/tmp/numbers/1/2/3/4/5" $strace_log && return 1
	grep -q "/tmp/numbers/1/2/3/4/5/6" $strace_log && return 1
	grep -q "/tmp/letters/a" $strace_log && return 1
	grep -q "/tmp/letters/a/b" $strace_log && return 1
	grep -q "/tmp/letters/a/b/c" $strace_log && return 1
	grep -q "/tmp/letters/a/b/c/d" $strace_log && return 1
	grep -q "/tmp/letters/a/b/c/d/e" $strace_log && return 1
	grep -q "/tmp/letters/a/b/c/d/e/f" $strace_log && return 1
	return 0
}

rm -rf /tmp/numbers
mkdir -p /tmp/numbers/1/2/3/4/5/6
rm -rf /tmp/letters
mkdir -p /tmp/letters/a/b/c/d/e/f
strace_log=$(mktemp)
strace -f -e openat -o $strace_log $OSCAP oval eval --results results.xml "$srcdir/test_probes_file_multiple_file_paths.xml"
ret=0
check_strace_output $strace_log || ret=$?
rm -f $strace_log
rm -f results.xml
rm -rf /tmp/numbers
rm -rf /tmp/letters
exit $ret
