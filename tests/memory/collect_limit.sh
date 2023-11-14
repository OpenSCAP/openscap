#!/bin/bash

set -e -o pipefail

. $builddir/tests/test_common.sh

# PROBE_ITEM_COLLECT_MAX limit is 1000
seq 1010 > /tmp/longfile
result=$(mktemp)
$OSCAP oval eval --results "$result" $srcdir/collect_limit.oval.xml
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@flag="incomplete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/message[@level="warning"]'
text="Object is incomplete because the object matches more than 1000 items."
assert_exists 1 "/oval_results/results/system/oval_system_characteristics/collected_objects/object/message[text()=\"$text\"]"
assert_exists 1000 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item'
rm -f /tmp/longfile
rm -f "$result"
