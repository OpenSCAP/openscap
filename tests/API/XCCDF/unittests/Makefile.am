
AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I$(top_srcdir)/src/CVSS/public \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CCE/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
		-I$(top_srcdir)/src/common/public \
		-I$(top_srcdir)/src/source/public \
		-I$(top_srcdir)/src/OVAL/probes/public \
		-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
		-I$(top_srcdir)/src \
		-I$(top_srcdir)/src/XCCDF_POLICY/public \
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/assume.h

check_DATA = not_executable
not_executable:
	touch not_executable

DISTCLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)
CLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)


TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh
check_PROGRAMS = \
	test_oscap_common \
	test_xccdf_shall_pass

test_oscap_common_SOURCES = test_oscap_common.c
test_oscap_common_SOURCES += $(top_srcdir)/src/common/util.c $(top_srcdir)/src/common/list.c $(top_srcdir)/src/common/alloc.c # This needs love (See trac#198)
test_oscap_common_CPPFLAGS = $(AM_CPPFLAGS) -DNDEBUG
test_xccdf_shall_pass_SOURCES = test_xccdf_shall_pass.c unit_helper.c

EXTRA_DIST += \
	all.sh \
	test_default_selector.oval.xml \
	test_default_selector.sh \
	test_default_selector.xccdf.xml \
	test_deriving_xccdf_result_from_oval.sh \
	test_deriving_xccdf_result_from_oval.xccdf.xml \
	test_deriving_xccdf_result_from_oval2.sh \
	test_deriving_xccdf_result_from_oval2.xccdf.xml \
	test_deriving_xccdf_result_from_oval_fail.oval.xml \
	test_deriving_xccdf_result_from_oval_multicheck.sh \
	test_deriving_xccdf_result_from_oval_multicheck.xccdf.xml \
	test_deriving_xccdf_result_from_oval_pass.oval.xml \
	test_fix_filtering.xccdf.xml \
	test_fix_filtering.sh \
	test_inherit_selector.oval.xml \
	test_inherit_selector.sh \
	test_inherit_selector.tailoring.xml \
	test_inherit_selector.xccdf.xml \
	test_unfinished.sh \
	test_unfinished.xccdf.xml \
	test_multiple_oval_files_with_same_basename.sh \
	test_multiple_oval_files_with_same_basename.xccdf.xml \
	test_oval_without_definition.oval.xml \
	test_oval_without_definition.sh \
	test_oval_without_definition.xccdf.xml \
	test_remediate_perl.sh \
	test_remediate_perl.xccdf.xml \
	test_remediate_python.sh \
	test_remediate_python.xccdf.xml \
	test_remediate_python_subs.sh \
	test_remediate_python_subs.xccdf.xml \
	test_remediate_simple.sh \
	test_remediate_simple.xccdf.xml \
	test_remediation_amp_escaping.sh \
	test_remediation_amp_escaping.xccdf.xml \
	test_remediation_bad_fix.sh \
	test_remediation_bad_fix.xccdf.xml \
	test_remediation_cdata.sh \
	test_remediation_cdata.xccdf.xml \
	test_remediation_fix_without_system.sh \
	test_remediation_fix_without_system.xccdf.xml \
	test_remediation_simple.oval.xml \
	test_remediation_simple.sh \
	test_remediation_simple.xccdf.xml \
	test_remediation_subs_plain_text.sh \
	test_remediation_subs_plain_text.xccdf.xml \
	test_remediation_subs_plain_text_empty.sh \
	test_remediation_subs_plain_text_empty.xccdf.xml \
	test_remediation_subs_unresolved.sh \
	test_remediation_subs_unresolved.xccdf.xml \
	test_remediation_subs_value_refine_value.sh \
	test_remediation_subs_value_refine_value.xccdf.xml \
	test_remediation_subs_value_take_first.sh \
	test_remediation_subs_value_take_first.xccdf.xml \
	test_remediation_subs_value_title.sh \
	test_remediation_subs_value_title.xccdf.xml \
	test_remediation_subs_value_without_selector.sh \
	test_remediation_subs_value_without_selector.xccdf.xml \
	test_remediation_xml_comments.sh \
	test_remediation_xml_comments.xccdf.xml \
	test_report_anaconda_fixes.sh \
	test_report_anaconda_fixes.tailoring.xml \
	test_report_anaconda_fixes.xccdf.xml \
	test_report_anaconda_fixes_ds.sh \
	test_report_check_with_empty_selector.oval.xml.result.xml \
	test_report_check_with_empty_selector.sh \
	test_report_check_with_empty_selector.xccdf.xml.result.xml \
	test_report_without_oval_poses_no_errors.sh \
	test_report_without_oval_poses_no_errors.xccdf.xml.result.xml \
	test_report_without_xsl_fails_gracefully.sh \
	test_xccdf_fix_attr_export.sh \
	test_xccdf_fix_attr_export.xccdf.xml \
	test_xccdf_complex_check_and_notchecked.sh \
	test_xccdf_complex_check_and_notchecked.xccdf.xml \
	test_xccdf_complex_check_nand.xccdf.xml \
	test_xccdf_complex_check_single_negate.xccdf.xml \
	test_xccdf_check_content_ref_without_name_attr.oval.xml \
	test_xccdf_check_content_ref_without_name_attr.sh \
	test_xccdf_check_content_ref_without_name_attr.xccdf.xml \
	test_xccdf_check_without_content_refs.sh \
	test_xccdf_check_without_content_refs.xccdf.xml \
	test_xccdf_check_negate.sh \
	test_xccdf_check_negate.xccdf.xml \
	test_xccdf_check_multi_check.sh \
	test_xccdf_check_multi_check.xccdf.xml \
	test_xccdf_check_multi_check2.sh \
	test_xccdf_check_multi_check2.xccdf.xml \
	test_xccdf_check_multi_check_zero_definitions.sh \
	test_xccdf_check_multi_check_zero_definitions.xccdf.xml \
	test_xccdf_check_processing_complex_priority.sh \
	test_xccdf_check_processing_complex_priority.xccdf.xml \
	test_xccdf_check_processing_invalid_content_refs.sh \
	test_xccdf_check_processing_invalid_content_refs.xccdf.xml \
	test_xccdf_check_processing_selector_bad.sh \
	test_xccdf_check_processing_selector_bad.xccdf.xml \
	test_xccdf_check_processing_selector_empty.sh \
	test_xccdf_check_processing_selector_empty.xccdf.xml \
	test_xccdf_check_unsupported_check_system.sh \
	test_xccdf_check_unsupported_check_system.xml \
	test_xccdf_multiple_testresults.sh \
	test_xccdf_multiple_testresults.xccdf.xml \
	test_xccdf_notchecked_has_check.sh \
	test_xccdf_notchecked_has_check.xccdf.xml \
	test_xccdf_overlaping_IDs.xccdf.xml \
	test_xccdf_refine_rule.sh \
	test_xccdf_refine_rule.xccdf.xml \
	test_xccdf_selectors_cluster1.sh \
	test_xccdf_selectors_cluster1.xccdf.xml \
	test_xccdf_selectors_cluster2.sh \
	test_xccdf_selectors_cluster2.xccdf.xml \
	test_xccdf_selectors_cluster3.sh \
	test_xccdf_selectors_cluster3.xccdf.xml \
	test_xccdf_sub_title.sh \
	test_xccdf_sub_title.xccdf.xml \
	test_xccdf_xml_escaping_value.sh \
	test_xccdf_xml_escaping_value.xccdf.xml \
	oval \
	oval_without_definitions.oval.xml \
	unit_helper.c \
	unit_helper.h
# HELLO PAL, THIS LIST IS ALPHABETICALLY SORTED.

