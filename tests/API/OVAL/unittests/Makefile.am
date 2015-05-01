DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*


TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	test_applicability_check.sh \
	applicability_check.xml \
	test_empty_filename.sh \
	empty_filename.xml \
	test_deprecated_def.sh \
	deprecated_def.xml \
	test_count_function.sh \
	oval-def_count_function.xml \
	comment.xml \
	test_comment.sh \
	test_int_comparison.oval.xml \
	test_int_comparison.sh \
	test_int_comparison.syschar.xml \
	test_ipv4_comparison.oval.xml \
	test_ipv4_comparison.sh \
	test_ipv4_comparison.syschar.xml \
	test_ipv4_subset_of.oval.xml \
	test_ipv4_subset_of.sh \
	test_ipv4_subset_of.syschar.xml \
	test_ipv4_super_set_of.oval.xml \
	test_ipv4_super_set_of.sh \
	test_ipv4_super_set_of.syschar.xml \
	test_ipv6_subset_of.oval.xml \
	test_ipv6_subset_of.sh \
	test_ipv6_subset_of.syschar.xml \
	test_ipv6_super_set_of.oval.xml \
	test_ipv6_super_set_of.sh \
	test_ipv6_super_set_of.syschar.xml \
	test_invalid_regex.sh \
	test_invalid_regex.xml \
	test_envvar_insensitive_equals.oval.xml \
	test_envvar_insensitive_equals.sh \
	test_envvar_insensitive_equals.syschar.xml \
	test_evr_string_missing_epoch.oval.xml \
	test_evr_string_missing_epoch.sh \
	test_evr_string_missing_epoch.syschar.xml \
	test_evr_string_comparison.oval.xml \
	test_evr_string_comparison.sh \
	test_evr_string_comparison.syschar.xml \
	test_filecontent_line.oval.xml \
	test_filecontent_line.sh \
	test_filecontent_line.syschar.xml \
	test_float_comparison.oval.xml \
	test_float_comparison.sh \
	test_float_comparison.syschar.xml \
	test_glob_to_regex.sh \
	test_glob_to_regex.xml \
	test_oval_empty_variable_evaluation.sh \
	test_oval_empty_variable_evaluation.xml \
	test_xmlns_missing.oval.xml \
	test_xmlns_missing.sh \
	test_xsinil_envv58_pid.oval.xml \
	test_xsinil_envv58_pid.sh \
	test_item_not_exist.sh \
	item_not_exist.xml \
	anyxmlsyschar.xml \
	anyxmloval.xml \
	test_anyxml.sh

