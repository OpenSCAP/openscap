#!/bin/bash

set -e -o pipefail

. $builddir/tests/test_common.sh

export OSCAP_PROBE_MAX_COLLECTED_ITEMS=100
seq 110 > /tmp/longfile
result=$(mktemp)
$OSCAP oval eval --results "$result" $srcdir/collect_limit.oval.xml
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@flag="incomplete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/message[@level="warning"]'
text="Object is incomplete because the object matches more than 100 items."
assert_exists 1 "/oval_results/results/system/oval_system_characteristics/collected_objects/object/message[text()=\"$text\"]"
assert_exists 100 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item'
rm -f /tmp/longfile
rm -f "$result"
