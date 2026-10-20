#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

probecheck "yamlfilecontent" || exit 255

oval_file="${srcdir}/test_probes_yamlfilecontent_content.xml"
result="$(mktemp results.XXXXXXX.xml)"

[ -f $result ] && rm -f $result

$OSCAP oval eval --results $result $oval_file

[ -f $result ]

sd='/oval_results/results/system/oval_system_characteristics/system_data'

assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="foo" and text()="bar"]'
assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="#" and @datatype="boolean" and text()="true"]'
assert_exists 2 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="#" and text()="<value>"]'

rm -f "$result"
