#!/bin/bash

set -e
set -o pipefail

if [ -f /bin/rpm ] && /bin/rpm -q libxml2; then
	# workaround trac#346: skip this test
	version="`/bin/rpm -q --qf '%{VERSION}\n' libxml2 | tail -n 1`";
	if echo $version | \
			awk -F. '{print ($1 == 2 && ($2 < 7 || ($2 == 7 && $3 < 7))) ? "true" : "false"}' | \
			grep true; then
		exit 0;
	fi
fi

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.err.XXXXXX)
echo "Stderr file = $stderr"

ret=0
$OSCAP info ${srcdir}/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 1 ]
[ -f $stderr ]
[ -s $stderr ]
cat $stderr | tail -n +4 | grep '^OpenSCAP Error:'

rm $stderr
