#!/bin/bash

# Check that variables have their values recorded in the OVAL results.
# This test verifies that different flavours of variable which are
# referenced from various OVAL elements are recorded in the
# 'tested_variable' and 'variable_value' elements.

. $builddir/tests/test_common.sh

set -e -o pipefail

function report_variable_values() {
	name="report_variable_values"
	defxml=${name}.def.xml
	varxml=${name}.var.xml
	resxml=$(mktemp ${name}.res.XXXXXX.xml)
	stderr=$(mktemp ${name}.stderr.XXXXXX)

	echo "Results file: $resxml"
	echo "Stderr dump from oscap: $stderr"

	echo "Running eval"
	$OSCAP oval eval --variables $srcdir/$varxml --results $resxml $srcdir/$defxml > $stderr 2>&1

	echo "Verifying results"
	result="$resxml"
	assert_exists 1 '/oval_results'
	assert_exists 1 '/oval_results/results'
	assert_exists 1 '/oval_results/results/system'
	assert_exists 1 '/oval_results/results/system/definitions'
	assert_exists 1 '/oval_results/results/system/definitions/definition[@result="true"]'
	assert_exists 1 '/oval_results/results/system/tests'
	assert_exists 8 '/oval_results/results/system/tests/test'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1" and @result="true"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/tested_item'
	assert_exists 2 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/tested_variable[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:1"]/tested_variable[@variable_id="oval:x:var:1" and text()="1"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2" and @result="true"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/tested_item'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:2"]/tested_variable[@variable_id="oval:x:var:2" and text()="2"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3" and @result="true"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_item'
	assert_exists 4 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_variable[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_variable[@variable_id="oval:x:var:1" and text()="1"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_variable[@variable_id="oval:x:var:2" and text()="2"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:3"]/tested_variable[@variable_id="oval:x:var:3" and text()="3"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4" and @result="true"]'
	assert_exists 0 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_item'
	assert_exists 4 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_variable[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_variable[@variable_id="oval:x:var:1" and text()="1"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_variable[@variable_id="oval:x:var:3" and text()="3"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:4"]/tested_variable[@variable_id="oval:x:var:4"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5" and @result="true"]'
	assert_exists 0 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_item'
	assert_exists 5 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_variable[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_variable[@variable_id="oval:x:var:1" and text()="1"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_variable[@variable_id="oval:x:var:2" and text()="2"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_variable[@variable_id="oval:x:var:3" and text()="3"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:5"]/tested_variable[@variable_id="oval:x:var:4"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6" and @result="true"]'
	assert_exists 2 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_item'
	assert_exists 5 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_variable[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_variable[@variable_id="oval:x:var:1" and text()="1"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_variable[@variable_id="oval:x:var:2" and text()="2"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_variable[@variable_id="oval:x:var:5" and text()="4"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:6"]/tested_variable[@variable_id="oval:x:var:5" and text()="5"]'

	assert_exists 0 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:7"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:8" and @result="true"]'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:8"]/tested_item'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:8"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:8"]/tested_variable[@variable_id="oval:x:var:2" and text()="2"]'

	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:9" and @result="true"]'
	assert_exists 2 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:9"]/tested_item'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:9"]/tested_variable'
	assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:x:tst:9"]/tested_variable[@variable_id="oval:x:var:2" and text()="2"]'

	assert_exists 1 '/oval_results/results/system/oval_system_characteristics'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects'
	assert_exists 7 '/oval_results/results/system/oval_system_characteristics/collected_objects/object'
	CO='/oval_results/results/system/oval_system_characteristics/collected_objects'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:1" and @flag="complete"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:1"]/reference[@item_ref]'
	assert_exists 2 "$CO"'/object[@id="oval:x:obj:1"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:1"]/variable_value[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:1"]/variable_value[@variable_id="oval:x:var:1" and text()="1"]'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:2" and @flag="complete"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:2"]/reference[@item_ref]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:2"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:2"]/variable_value[@variable_id="oval:x:var:2" and text()="2"]'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:3" and @flag="complete"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:3"]/reference[@item_ref]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:3"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:3"]/variable_value[@variable_id="oval:x:var:3" and text()="3"]'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:4" and @flag="does not exist"]'
	assert_exists 0 "$CO"'/object[@id="oval:x:obj:4"]/reference[@item_ref]'
	assert_exists 4 "$CO"'/object[@id="oval:x:obj:4"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:4"]/variable_value[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:4"]/variable_value[@variable_id="oval:x:var:1" and text()="1"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:4"]/variable_value[@variable_id="oval:x:var:3" and text()="3"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:4"]/variable_value[@variable_id="oval:x:var:4"]'

	assert_exists 0 "$CO"'/object[@id="oval:x:obj:5"]'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:6" and @flag="complete"]'
	assert_exists 2 "$CO"'/object[@id="oval:x:obj:6"]/reference[@item_ref]'
	assert_exists 5 "$CO"'/object[@id="oval:x:obj:6"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:6"]/variable_value[@variable_id="oval:x:var:1" and text()="0"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:6"]/variable_value[@variable_id="oval:x:var:1" and text()="1"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:6"]/variable_value[@variable_id="oval:x:var:2" and text()="2"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:6"]/variable_value[@variable_id="oval:x:var:5" and text()="4"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:6"]/variable_value[@variable_id="oval:x:var:5" and text()="5"]'

	assert_exists 0 "$CO"'/object[@id="oval:x:obj:7"]'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:8" and @flag="complete"]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:8"]/reference[@item_ref]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:8"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:8"]/variable_value[@variable_id="oval:x:var:2" and text()="2"]'

	assert_exists 1 "$CO"'/object[@id="oval:x:obj:9" and @flag="complete"]'
	assert_exists 2 "$CO"'/object[@id="oval:x:obj:9"]/reference[@item_ref]'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:9"]/variable_value'
	assert_exists 1 "$CO"'/object[@id="oval:x:obj:9"]/variable_value[@variable_id="oval:x:var:2" and text()="2"]'

	rm $resxml
	rm $stderr
}

test_init report_variable_values.log
test_run "Report variable values in OVAL results" report_variable_values
test_exit
