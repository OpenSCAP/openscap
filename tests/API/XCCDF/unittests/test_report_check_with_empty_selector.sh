#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

	# Workaround trac#245 for distcheck
	oval=${name}.oval.xml.result.xml
	ln -s $srcdir/$oval $oval
	# Workaround end

$OSCAP xccdf generate --format html report --output $result $srcdir/${name}.xccdf.xml.result.xml 2> $stderr

	# Workaround
	rm -rf $oval
	unset oval
	# Workaround end

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm -rf $stderr

grep '<div class="oval-results">' $result
grep 'Testing file permissions of /etc/shadow' $result
grep '<code>--------- </code>' $result
rm -rf $result
