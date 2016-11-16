#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
echo "Stderr file = $stderr"
echo "Result file = $result"

line1='^\W*part /tmp$'
line2='^\W*part /tmp --mountoptions=nodev$'
line3='^\W*passwd --minlen=14$'

$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
grep "$line1" $result && true
grep "$line2" $result && true
grep "$line3" $result && false
:> $result

$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
grep "$line1" $result && true
grep "$line2" $result && true
grep "$line3" $result && true
:> $result

$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--cpe $srcdir/cpe-dict.xml \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
grep "$line1" $result
grep "$line2" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"
[ "`grep -v "$line1" $result | grep -v "$line2" | sed 's/\W//g'`"x == x ]

# To test permissions of newly created files
rm $result
result=./${name}.out
[ -f $result ] && rm $result

$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--cpe $srcdir/cpe-dict.xml \
	--profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
stat $result | grep 'Access: (..../-rw.------)'
grep "$line1" $result
grep "$line2" $result
grep "$line3" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"
[ "`grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3" | sed 's/\W//g'`"x == x ]
rm $result
