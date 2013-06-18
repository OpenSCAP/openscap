#!/bin/bash
#
# Copyright 2013 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# This script is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

set -e -o pipefail

. $srcdir/../../../test_common.sh

assert_exists() { [ $($XPATH $result 'count('"$2"')') == "$1" ]; }
export -f assert_exists

#
# Export two values from XCCDF to a single OVAL variable as a part
# of single rule. Result is a single varible set with multiple values.
#
function xccdf_export_1_multival() {
	local name=$FUNCNAME
	local variables="requires_both-oval.xml-0.variables-0.xml"
	local result="$variables"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $result ] || rm $result
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_1 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 2 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	rm $variables
	rm $stderr
}

#
# Export two values from XCCDF to a single OVAL variable that it should
# result in multiple (two) variable sets each with a single value.
#
function xccdf_export_2_multiset(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local variables2="requires_both-oval.xml-0.variables-2.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	[ ! -f $variables2 ] || rm $variables2
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_2 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ -f $variables1 ]
	[ ! -f $variables2 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 1 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	result="$variables1"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 1 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	rm $stderr
	rm $variables0
	rm $variables1
}

#
# Export a single value from XCCDF to a single OVAL variable twice.
#
function xccdf_export_3_twice_same(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_3 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ ! -f $variables1 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 1 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	rm $stderr
	rm $variables0
}

#
# Export the very same value twice in the same batch. The behaviour seems
# undefined, although OpenSCAP chooses to export a single value to OVAL
#
function xccdf_export_4_two_same(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_4 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ ! -f $variables1 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 1 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	rm $stderr
	rm $variables0
}

#
# Export two variable sets (the very same) from XCCDF to OVAL.
#
function xccdf_export_5_multival_twice(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_5 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ ! -f $variables1 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 2 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	rm $stderr
	rm $variables0
}

#
# Export two distinct variable sets from XCCDF to OVAL.
#
function xccdf_export_6_multiset_multival(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local variables2="requires_both-oval.xml-0.variables-2.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	[ ! -f $variables2 ] || rm $variables2
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_6 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ -f $variables1 ]
	[ ! -f $variables2 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 2 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	result="$variables1"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 2 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="200"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="400"]'
	rm $stderr
	rm $variables0
	rm $variables1
}

#
# Export the very same variable sets twice (the second value-set is shuffled
# but the tool should correctly recognize it is the same
#
function xccdf_export_7_shuffled_multival(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_7 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ ! -f $variables1 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 4 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="200"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="400"]'
	rm $stderr
	rm $variables0
}

#
# Export the very same variable sets twice (the second value-set is shuffled
# and a different set of values is duplicated in each bunch.
#
function xccdf_export_8_shuffled_multival(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_8 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ ! -f $variables1 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 3 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="200"]'
	rm $stderr
	rm $variables0
}

#
# Export two multivals, the first is subset of the second or third.
#
function xccdf_export_9_first_subset(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local variables2="requires_both-oval.xml-0.variables-2.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	[ ! -f $variables2 ] || rm $variables2
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_9 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ -f $variables1 ]
	[ ! -f $variables2 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 2 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	result="$variables1"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 3 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="200"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	rm $stderr
	rm $variables0
	rm $variables1
}

#
# Export two multivals, the last is subset of the first two.
#
function xccdf_export_A_second_subset(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local variables2="requires_both-oval.xml-0.variables-2.xml"
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	echo "Stderr file = $stderr"

	[ ! -f $variables0 ] || rm $variables0
	[ ! -f $variables1 ] || rm $variables1
	[ ! -f $variables2 ] || rm $variables2
	$OSCAP xccdf export-oval-variables --profile xccdf_moc.elpmaxe.www_profile_10 \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2>&1 > $stderr
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ -f $variables1 ]
	[ ! -f $variables2 ]
	local result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 3 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="200"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	result="$variables1"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 2 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	rm $stderr
	rm $variables0
	rm $variables1
}

#
# Evaluate XCCDF while exporting two values from XCCDF document to a single OVAL
# variable that it should result in multiple (two) variable sets each with a single
# value. This tests asserts for correctly collected system characteristics.
#
function xccdf_eval_2_multiset(){
	local variables0="requires_both-oval.xml-0.variables-0.xml"
	local variables1="requires_both-oval.xml-0.variables-1.xml"
	local variables2="requires_both-oval.xml-0.variables-2.xml"
	local oval_result="requires_both-oval.xml.result.xml"
	local xccdf_result=$(mktemp -t ${FUNCNAME}.xml.XXXXXX)
	local stderr=$(mktemp -t ${FUNCNAME}.err.XXXXXX)
	local profile="xccdf_moc.elpmaxe.www_profile_2"
	local tested_file="testing_file.xml"
	echo "Stderr file = $stderr"
	cp $srcdir/testing_file_300.xml $tested_file

	for f in $variables0 $variables1 $variables2 $oval_result $xccdf_result; do
		[ ! -f $f ] || rm $f
	done
	local res=0
	$OSCAP xccdf eval --profile $profile \
		--export-variables --oval-results --results $xccdf_result \
		$srcdir/test_xccdf_variable_instance.xccdf.xml 2> $stderr || res=$?
	[ $res -eq 2 ]
	[ -f $stderr ]; [ ! -s $stderr ]
	[ -f $variables0 ]
	[ -f $variables1 ]
	[ ! -f $variables2 ]
	local result="$xccdf_result"
	assert_exists 1 '/Benchmark/TestResult'
	assert_exists 1 '/Benchmark/TestResult/profile'
	assert_exists 1 '/Benchmark/TestResult/profile/@*'
	assert_exists 1 '/Benchmark/TestResult/profile[@idref="'$profile'"]'
	assert_exists 2 '/Benchmark/TestResult/rule-result/result[text()!="notselected"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result/result[text()="pass"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/result[text()="pass"]'
	assert_exists 0 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/message'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/check'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/check/check-export'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/check/check-export[@export-name="oval:com.example.www:var:1"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/check/check-export[@value-id="xccdf_moc.elpmaxe.www_value_1"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_2"]/check/check-content-ref'
	assert_exists 1 '/Benchmark/TestResult/rule-result/result[text()="fail"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/result[text()="fail"]'
	assert_exists 0 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/message'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/check'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/check/check-export'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/check/check-export[@export-name="oval:com.example.www:var:1"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/check/check-export[@value-id="xccdf_moc.elpmaxe.www_value_2"]'
	assert_exists 1 '/Benchmark/TestResult/rule-result[@idref="xccdf_moc.elpmaxe.www_rule_3"]/check/check-content-ref'
	result="$variables0"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 1 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="300"]'
	result="$variables1"
	assert_exists 1 '/oval_variables'
	assert_exists 1 '/oval_variables/variables'
	assert_exists 1 '/oval_variables/variables/variable'
	assert_exists 1 '/oval_variables/variables/variable[@id="oval:com.example.www:var:1"]'
	assert_exists 1 '/oval_variables/variables/variable[@datatype="string"]'
	assert_exists 1 '/oval_variables/variables/variable/value'
	assert_exists 1 '/oval_variables/variables/variable/value[text()="600"]'
	result="$oval_result"
	assert_exists 1 '/oval_results'
	assert_exists 1 '/oval_results/results'
	assert_exists 1 '/oval_results/results/system'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/generator'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_info'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item'
	assert_exists 5 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/*'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:filepath'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:path'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:filename'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:xpath'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:value_of'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:value_of[text()="300"]'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object'
	assert_exists 3 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/@*'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:com.example.www:obj:1"]'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@version="1"]'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@flag="complete"]'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/reference'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/reference/@*'
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/reference/@item_ref'
	assert_exists 4 '/oval_results/results/system/oval_system_characteristics/*'
	assert_exists 1 '/oval_results/results/system/tests'
	# TODO: OpenSCAP takes buggy behaviour, it exports only one test.
	# While there shall be two test, the first passing and another failing.
	assert_exists 1 '/oval_results/results/system/definitions'
	assert_exists 3 '/oval_results/results/system/*'
	rm $stderr
	rm $xccdf_result
	rm $oval_result
	rm $variables0
	rm $variables1
	chmod u+w $tested_file ; rm $tested_file
}

test_init test_api_xccdf_variable_instance.log
test_run "Export from XCCDF to variables: 1x2 values (multival)" xccdf_export_1_multival
test_run "Export from XCCDF to variables: 2x1 values (multiset)" xccdf_export_2_multiset
test_run "Export from XCCDF to variables: 1x2 same vales (none)" xccdf_export_4_two_same
test_run "Export from XCCDF to variables: 2x1 same value (none)" xccdf_export_3_twice_same
test_run "Export from XCCDF to variables: 2x2 values (multiset,multival)" xccdf_export_6_multiset_multival
test_run "Export from XCCDF to variables: 2x2 same values (multival)" xccdf_export_5_multival_twice
test_run "Export from XCCDF to variables: 2x4 same shuffled values (multival)" xccdf_export_7_shuffled_multival
test_run "Export from XCCDF to variables: 2x4 same shuffled repeating values (multival)" xccdf_export_8_shuffled_multival
test_run "Export from XCCDF to variables: 3x3 the first is subset (multiset,multival)" xccdf_export_9_first_subset
test_run "Export from XCCDF to variables: 3x3 the last is subset (multiset,multival)" xccdf_export_A_second_subset

test_run "Evaluate XCCDF: 2x1 values (multiset)" xccdf_eval_2_multiset

test_exit
