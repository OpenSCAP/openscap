#!/bin/bash

set -x
set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

	# Workaround trac#245 for distcheck
	oval=${name}.oval.xml.result.xml
	if [ ! -f "$oval" ]; then
		ln -s $srcdir/$oval $oval
	fi
	# Workaround end

$OSCAP xccdf generate --format html report --output $result $srcdir/${name}.xccdf.xml.result.xml 2> $stderr

	# Workaround
	if [ -L "$oval" ]; then
		rm $oval
	fi
	unset oval
	# Workaround end

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

LC_ALL=C grep '<div class="oval-results">' $result
LC_ALL=C grep 'Testing file permissions of /etc/shadow' $result
LC_ALL=C grep '<code>--------- </code>' $result
echo $result
if [ -f /usr/bin/perl ]; then
  LC_ALL=C /usr/bin/perl -w -0777 -p -e 's|<a href=".*">OpenSCAP</a>\s*\([0-9\.]+\)|XXMATCHEDXX|g' $result | grep XXMATCHEDXX
fi
rm $result
