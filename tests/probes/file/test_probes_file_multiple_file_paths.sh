#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

probecheck "file" || exit 255

case $(uname) in
	FreeBSD)
		which truss || exit 255
		tracer="$(which truss) -f -o"
		;;
	*)
		which strace || exit 255
		tracer="$(which strace) -f -e openat -o"
		;;
esac

function check_trace_output {
	trace_log="$1"
	grep -q "/tmp/numbers/1" $trace_log && return 1
	grep -q "/tmp/numbers/1/2" $trace_log && return 1
	grep -q "/tmp/numbers/1/2/3" $trace_log && return 1
	grep -q "/tmp/numbers/1/2/3/4" $trace_log && return 1
	grep -q "/tmp/numbers/1/2/3/4/5" $trace_log && return 1
	grep -q "/tmp/numbers/1/2/3/4/5/6" $trace_log && return 1
	grep -q "/tmp/letters/a" $trace_log && return 1
	grep -q "/tmp/letters/a/b" $trace_log && return 1
	grep -q "/tmp/letters/a/b/c" $trace_log && return 1
	grep -q "/tmp/letters/a/b/c/d" $trace_log && return 1
	grep -q "/tmp/letters/a/b/c/d/e" $trace_log && return 1
	grep -q "/tmp/letters/a/b/c/d/e/f" $trace_log && return 1
	return 0
}

rm -rf /tmp/numbers
mkdir -p /tmp/numbers/1/2/3/4/5/6
rm -rf /tmp/letters
mkdir -p /tmp/letters/a/b/c/d/e/f
trace_log=$(mktemp)
$tracer $trace_log $OSCAP oval eval --results results.xml "$srcdir/test_probes_file_multiple_file_paths.xml"
ret=0
check_trace_output $trace_log || ret=$?
rm -f $trace_log
rm -f results.xml
rm -rf /tmp/numbers
rm -rf /tmp/letters
exit $ret
