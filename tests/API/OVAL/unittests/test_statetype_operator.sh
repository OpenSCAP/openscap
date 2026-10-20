#!/usr/bin/env bash
. $builddir/tests/test_common.sh

result=`mktemp`

set -e
set -o pipefail

cat > /tmp/foo.txt <<EOF
Testing state operator.
EOF
chmod 0700 /tmp/foo.txt

$OSCAP oval eval --results $result $srcdir/statetype_operator.xml

assert_exists 11 '/oval_results/oval_definitions/states/unix-def:file_state'
assert_exists 1 '/oval_results/results/system/tests'
assert_exists 11 '/oval_results/results/system/tests/test'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2" and @result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3" and @result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5" and @result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:7" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:8" and @result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:9" and @result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:10" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:11" and @result="false"]'

rm $result
rm /tmp/foo.txt
