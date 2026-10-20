#!/usr/bin/env bash
. $builddir/tests/test_common.sh

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/empty_state.xml

# Definition 1: State with no entities (default AND operator) must evaluate to true
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/tested_item[@result="true"]'

# Definition 2: State with no entities and OR operator must evaluate to true
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:2" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2" and @result="true"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/tested_item[@result="true"]'

# Definition 3: Extended definition applicability check (Ubuntu USN pattern) must evaluate to true
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3" and @result="true"]'

# Definition 4: Non-existent item must still evaluate to false even when state has no entities
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4" and @result="false"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3" and @result="false"]'

rm $result
