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
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep "$line1" $result
grep "$line2" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"
[ "`grep -v "$line1" $result | grep -v "$line2" | sed 's/\W//g'`"x == x ]
:> $result

$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $result $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep "$line1" $result
grep "$line2" $result
grep "$line3" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"
[ "`grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3" | sed 's/\W//g'`"x == x ]
rm $result


# And Now For Something Completely Different -- Tailoring:
$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--tailoring-file $srcdir/${name}.tailoring.xml \
	--profile xccdf_org.open-scap_profile_unselecting \
	--output $result \
	$srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
[ "`cat $result | sed 's/\W//g'`"x == x ]
rm $result

line4='^\W*passwd --minlen=8$'
$OSCAP xccdf generate fix --template urn:redhat:anaconda:pre \
	--tailoring-file $srcdir/${name}.tailoring.xml \
	--profile xccdf_org.open-scap_profile_override \
	--output $result \
	$srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep "$line4" $result
[ "`grep -v $line4 $result | sed 's/\W//g'`"x == x ]
rm $result
