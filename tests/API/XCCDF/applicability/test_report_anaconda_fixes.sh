#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(make_temp_file /tmp ${name}.out)
stderr=$(make_temp_file /tmp ${name}.out)
echo "Stderr file = $stderr"
echo "Result file = $result"

line1='^\W*part /tmp$'
line2='^\W*part /tmp --mountoptions=nodev$'
line3='^\W*passwd --minlen=14$'

$OSCAP xccdf generate fix --fix-type anaconda \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
grep "$line1" $result
grep "$line2" $result
grep "$line3" $result && false
:> $result

$OSCAP xccdf generate fix --fix-type anaconda \
	--profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
grep "$line1" $result
grep "$line2" $result
grep "$line3" $result
:> $result

$OSCAP xccdf generate fix --fix-type anaconda \
	--cpe $srcdir/cpe-dict.xml \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]
grep "$line1" $result
grep "$line2" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"

[ "`grep -v "$line1" $result | grep -v "$line2" | xsed 's/\W//g'`"x == x ]

# To test permissions of newly created files
rm $result
result=./${name}.out
[ -f $result ] && rm $result

$OSCAP xccdf generate fix --fix-type anaconda \
	--cpe $srcdir/cpe-dict.xml \
	--profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

case $(uname) in
	FreeBSD)
		stat $result| tr ' ' '\n' | grep '^-rw'
		;;
	*)
		stat $result | grep 'Access: (..../-rw.------)'
		;;
esac

grep "$line1" $result
grep "$line2" $result
grep "$line3" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"

[ "`grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3" | xsed 's/\W//g'`"x == x ]

rm $result
