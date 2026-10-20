#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

name=$(basename $0 _ds.sh)
xccdf=${name}.xccdf.xml
sds=$(make_temp_file /tmp ${name}.sds)
stderr=$(make_temp_file /tmp ${name}_ds.err)
result=$(make_temp_file /tmp ${name}_ds.out)
cp $srcdir/test_report_anaconda_fixes.sds.xml $sds

echo "name: $name"
echo "sds file: $sds"
echo "Stderr file = $stderr"
echo "Results file = $result"

line1='^\W*part /tmp$'
line2='^\W*part /tmp --mountoptions=nodev$'
line3='^\W*passwd --minlen=14$'

$OSCAP ds sds-validate $sds

datastream_id=scap_org.open-scap_datastream_from_xccdf_test_report_anaconda_fixes.xccdf.xml
component_id=scap_org.open-scap_cref_test_report_anaconda_fixes.xccdf.xml

$OSCAP info $sds | grep $datastream_id
$OSCAP info $sds | grep $component_id

$OSCAP xccdf generate fix --fix-type anaconda \
	--datastream-id $datastream_id --xccdf-id $component_id \
	--output $result $sds 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep "$line1" $result
grep "$line2" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"

[ "`grep -v "$line1" $result | grep -v "$line2" | xsed 's/\W//g'`"x == x ]

:> $result

$OSCAP xccdf generate fix --fix-type anaconda \
	--profile xccdf_moc.elpmaxe.www_profile_1 \
	--output $result $sds 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr
grep "$line1" $result
grep "$line2" $result
grep "$line3" $result
grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3"

[ "`grep -v "$line1" $result | grep -v "$line2" | grep -v "$line3" | xsed 's/\W//g'`"x == x ]

rm $result $sds
