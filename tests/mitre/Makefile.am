DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_mitre.sh

EXTRA_DIST =	test_mitre.sh \
		support.tgz \
		ind-def_environmentvariable58_test.xml \
		ind-def_environmentvariable_test.xml \
		ind-def_family_test.xml \
		ind-def_filehash58_test.xml \
		ind-def_filehash_test.xml \
		ind-def_ldap_test.xml \
		ind-def_sql_test.xml \
		ind-def_textfilecontent54_test.xml \
		ind-def_textfilecontent_test.xml \
		ind-def_unknown_test.xml \
		ind-def_variable_test.xml \
		ind-def_xmlfilecontent_test.xml \
		linux-def_dpkginfo_test.xml \
		linux-def_inetlisteningservers_test.xml \
		linux-def_partition_test.xml \
		linux-def_rpminfo_test.xml \
		linux-def_rpmverify_test.xml \
		linux-def_selinuxboolean_test.xml \
		linux-def_selinuxsecuritycontext_test.xml \
		linux-def_slackwarepkginfo_test.xml \
		oval_binary_datatype.xml \
		oval_boolean_datatype.xml \
		oval_check_enumeration_entity.xml \
		oval_check_enumeration_object_state.xml \
		oval_check_enumeration_variable_values.xml \
		oval-def_arithmetic_function.xml \
		oval-def_begin_function.xml \
		oval-def_concat_function.xml \
		oval-def_constant_variable.xml \
		oval-def_criteria.xml \
		oval-def_criterion.xml \
		oval-def_end_function.xml \
		oval-def_escape_regex_function.xml \
		oval-def_extend_definition.xml \
		oval-def_external_variable.xml \
		oval-def_literal_component.xml \
		oval-def_local_variable.xml \
		oval-def_object_component.xml \
		oval-def_regex_capture_function.xml \
		oval-def_set.xml \
		oval-def_split_function.xml \
		oval-def_substring_function.xml \
		oval-def_time_difference_function.xml \
		oval-def_variable_component.xml \
		oval_evr_string_datatype.xml \
		oval_existence_enumeration.xml \
		oval_float_datatype.xml \
		oval_int_datatype.xml \
		oval_string_datatype.xml \
		oval_version_datatype.xml \
		unix-def_file_test.xml \
		unix-def_inetd_test.xml \
		unix-def_interface_test.xml \
		unix-def_password_test.xml \
		unix-def_process58_test.xml \
		unix-def_process_test.xml \
		unix-def_runlevel_test.xml \
		unix-def_shadow_test.xml \
		unix-def_uname_test.xml \
		unix-def_xinetd_test.xml
