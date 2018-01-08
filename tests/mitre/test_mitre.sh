#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>
#	   Daniel Kopecek <dkopecek@redhat.com>


. $builddir/tests/test_common.sh
. $srcdir/test_mitre_common.sh

# Test Cases.
test_init "test_mitre.log"

# does not work because of symplink `/etc/rc' -> `/etc/rc.d/rc' (oval:org.mitre.oval.test:tst:102)
#test_run "unix-def_file_test.xml" test_mitre unix-def_file_test.xml "true"

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

test_run "unix-def_password_test.xml" test_mitre unix-def_password_test.xml "true"

# these are outdated
#test_run "unix-def_process58_test.xml" test_mitre unix-def_process58_test.xml "true"
#test_run "unix-def_process_test.xml" test_mitre unix-def_process_test.xml "true"

# Fedora 16 and RHEL-7 - no runlevel
if [[ ( ${DISTRO#Fedora} != "$DISTRO" && $DISTRO_RELEASE -lt 16 ) || \
	( ${DISTRO#Red Hat} != "$DISTRO" && $DISTRO_RELEASE -lt 7 ) ]]; then
	test_run "unix-def_runlevel_test.xml" test_mitre unix-def_runlevel_test.xml "true"
fi

test_run "unix-def_uname_test.xml" test_mitre unix-def_uname_test.xml "true"

# needs only two interfaces - lo, eth0 - without ipv6 addresses
#test_run "unix-def_interface_test.xml" test_mitre unix-def_interface_test.xml "true"

# root needed
if [[ $(id -u) == 0 ]]; then
	test_run "unix-def_shadow_test.xml" test_mitre unix-def_shadow_test.xml "true"
fi

# install xinetd, telnet-server and tftp-server in order to test xinetd probe
if [[ -f "/etc/xinetd.conf" && -f "/etc/xinetd.d/tftp" && -f "/etc/xinetd.d/telnet" ]]; then
	test_run "unix-def_xinetd_test.xml" test_mitre unix-def_xinetd_test.xml "true"
fi

# Unsupported objects on Fedora
#test_run "unix-def_inetd_test.xml" test_mitre unix-def_inetd_test.xml "unknown"

test_exit cleanup_mitre
