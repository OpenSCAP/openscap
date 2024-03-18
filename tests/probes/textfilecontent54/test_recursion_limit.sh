#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

probecheck "textfilecontent54" || exit 255

cp $srcdir/30-ospp-v42.rules /tmp

name=$(basename $0 .sh)
input=$srcdir/$name.oval.xml
result=$(mktemp)
stdout=$(mktemp)
stderr=$(mktemp)

$OSCAP oval eval --results $result $input > $stdout 2> $stderr

grep -q "Function oscap_pcre_exec() failed to match a regular expression with return code -21" $stderr

assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="error"]'

co='/oval_results/results/system/oval_system_characteristics/collected_objects'
assert_exists 1 $co'/object[@flag="error"]'
assert_exists 1 $co'/object/message[@level="error"]'
assert_exists 1 $co'/object/message[text()="Regular expression pattern match failed in file /tmp/30-ospp-v42.rules with error -21."]'

rm -f /tmp/30-ospp-v42.rules
rm -f $result
rm -f $stdout
rm -f $stderr
