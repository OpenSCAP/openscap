#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

cat > /tmp/foo.txt <<EOF
Hello
Hello
Hello
Hello
Hello
EOF

$OSCAP oval eval --results $result $srcdir/state_check_existence.xml

assert_exists 4 '/oval_results/oval_definitions/states/ind-def:textfilecontent54_state/ind-def:text'
assert_exists 3 '/oval_results/oval_definitions/states/ind-def:textfilecontent54_state/ind-def:text[@check_existence]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:textfilecontent54_state/ind-def:text[@check_existence="all_exist"]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:textfilecontent54_state/ind-def:text[@check_existence="none_exist"]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:textfilecontent54_state/ind-def:text[@check_existence="only_one_exists"]'
assert_exists 1 '/oval_results/results/system/tests'
assert_exists 4 '/oval_results/results/system/tests/test'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1" and @result="true"]'
assert_exists 5 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/tested_item[@result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2" and @result="false"]'
assert_exists 5 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/tested_item[@result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3" and @result="false"]'
assert_exists 5 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_item[@result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4" and @result="true"]'
assert_exists 5 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_item[@result="true"]'
assert_exists 5 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item'

rm $result
rm /tmp/foo.txt
