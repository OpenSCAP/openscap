#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>
#	   Daniel Kopecek <dkopecek@redhat.com>


. $builddir/tests/test_common.sh
. $srcdir/test_mitre_common.sh

# Test Cases.
test_init "test_mitre_oval.log"

test_run "oval_binary_datatype.xml" test_mitre oval_binary_datatype.xml "true"
test_run "oval_boolean_datatype.xml" test_mitre oval_boolean_datatype.xml "true"
test_run "oval_check_enumeration_entity.xml" test_mitre oval_check_enumeration_entity.xml "true"
test_run "oval_check_enumeration_object_state.xml" test_mitre oval_check_enumeration_object_state.xml "true"
test_run "oval_check_enumeration_variable_values.xml" test_mitre oval_check_enumeration_variable_values.xml "true"
test_run "oval-def_arithmetic_function.xml" test_mitre oval-def_arithmetic_function.xml "true"
test_run "oval-def_begin_function.xml" test_mitre oval-def_begin_function.xml "true"
test_run "oval-def_concat_function.xml" test_mitre oval-def_concat_function.xml "true"
test_run "oval-def_constant_variable.xml" test_mitre oval-def_constant_variable.xml "true"
test_run "oval-def_criteria.xml" test_mitre oval-def_criteria.xml "true"
test_run "oval-def_criterion.xml" test_mitre oval-def_criterion.xml "true"
test_run "oval-def_end_function.xml" test_mitre oval-def_end_function.xml "true"
test_run "oval-def_escape_regex_function.xml" test_mitre oval-def_escape_regex_function.xml "true"
test_run "oval-def_extend_definition.xml" test_mitre oval-def_extend_definition.xml "oval:org.mitre.oval.test:def:117" "true" "oval:org.mitre.oval.test:def:97" "true"
test_run "oval-def_external_variable.xml" test_mitre oval-def_external_variable.xml "true"
test_run "oval-def_literal_component.xml" test_mitre oval-def_literal_component.xml "true"
test_run "oval-def_local_variable.xml" test_mitre oval-def_local_variable.xml "true"
test_run "oval-def_object_component.xml" test_mitre oval-def_object_component.xml "true"
test_run "oval-def_regex_capture_function.xml" test_mitre oval-def_regex_capture_function.xml "true"
# this test fails on filesystems without relatime - directory access time is changed every time 
# is readed
if (grep " /tmp " /proc/mounts || grep " / " /proc/mounts) | grep -q relatime; then
	test_run "oval-def_set.xml" test_mitre oval-def_set.xml "true"
fi
test_run "oval-def_split_function.xml" test_mitre oval-def_split_function.xml "true"

# win_filetime format is currently unsupported, there are possibly some wrong values in the document
#test_run "oval-def_time_difference_function.xml" test_mitre oval-def_time_difference_function.xml "true"

test_run "oval-def_substring_function.xml" test_mitre oval-def_substring_function.xml "true"
test_run "oval-def_variable_component.xml" test_mitre oval-def_variable_component.xml "true"
test_run "oval_evr_string_datatype.xml" test_mitre oval_evr_string_datatype.xml "true"
test_run "oval_existence_enumeration.xml" test_mitre oval_existence_enumeration.xml "true"
test_run "oval_float_datatype.xml" test_mitre oval_float_datatype.xml "true"
test_run "oval_int_datatype.xml" test_mitre oval_int_datatype.xml "true"
test_run "oval_string_datatype.xml" test_mitre oval_string_datatype.xml "true"
test_run "oval_version_datatype.xml" test_mitre oval_version_datatype.xml "true"

test_exit cleanup_mitre
