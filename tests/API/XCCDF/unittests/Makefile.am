AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I$(top_srcdir)/src/CVSS/public \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CCE/public \
		-I$(top_srcdir)/src/DS/public \
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


check_DATA += not_executable
not_executable:
	touch not_executable


DISTCLEANFILES += $(check_DATA)
CLEANFILES += $(check_DATA)


TESTS += %reldir%/all.sh
check_PROGRAMS = \
	test_oscap_common \
	test_xccdf_overrides \
	test_xccdf_shall_pass

test_oscap_common_SOURCES = %reldir%/test_oscap_common.c
test_oscap_common_SOURCES += $(top_srcdir)/src/common/util.c $(top_srcdir)/src/common/list.c $(top_srcdir)/src/common/alloc.c # This needs love (See trac#198)
test_oscap_common_CPPFLAGS = $(AM_CPPFLAGS) -DNDEBUG
test_xccdf_shall_pass_SOURCES = %reldir%/test_xccdf_shall_pass.c %reldir%/unit_helper.c
test_xccdf_overrides_SOURCES = %reldir%/test_xccdf_overrides.c

EXTRA_DIST += \
	$(top_srcdir)/tests/assume.h \
	%reldir%/all.sh \
	%reldir%/oval \
	%reldir%/oval_without_definitions.oval.xml \
	%reldir%/test_default_selector.oval.xml \
	%reldir%/test_default_selector.sh \
	%reldir%/test_default_selector.xccdf.xml \
	%reldir%/test_deriving_xccdf_result_from_oval.sh \
	%reldir%/test_deriving_xccdf_result_from_oval.xccdf.xml \
	%reldir%/test_deriving_xccdf_result_from_oval2.sh \
	%reldir%/test_deriving_xccdf_result_from_oval2.xccdf.xml \
	%reldir%/test_deriving_xccdf_result_from_oval_fail.oval.xml \
	%reldir%/test_deriving_xccdf_result_from_oval_multicheck.sh \
	%reldir%/test_deriving_xccdf_result_from_oval_multicheck.xccdf.xml \
	%reldir%/test_deriving_xccdf_result_from_oval_pass.oval.xml \
	%reldir%/test_empty_variable.oval.xml \
	%reldir%/test_empty_variable.sh \
	%reldir%/test_empty_variable.xccdf.xml \
	%reldir%/test_fix_arf.playbook1.yml \
	%reldir%/test_fix_arf.playbook2.yml \
	%reldir%/test_fix_arf.xccdf.xml \
	%reldir%/test_fix_arf.sh \
	%reldir%/test_fix_instance.oval.xml \
	%reldir%/test_fix_instance.sh \
	%reldir%/test_fix_instance.xccdf.xml \
	%reldir%/test_fix_filtering.xccdf.xml \
	%reldir%/test_fix_filtering.sh \
	%reldir%/test_fix_resultid_by_suffix.sh \
	%reldir%/test_fix_resultid_by_suffix.xml \
	%reldir%/test_fix_script_header.sh \
	%reldir%/test_fix_script_header.xccdf.xml \
	%reldir%/test_generate_fix_ansible_vars.sh \
	%reldir%/test_generate_fix_ansible_vars_ds.xml \
	%reldir%/test_generate_fix_ansible_vars_ds-tailoring.xml \
	%reldir%/test_generate_fix_ansible_vars_golden.yml \
	%reldir%/test_generate_fix_ansible_vars_golden_altered.yml \
	%reldir%/test_generate_fix_ansible_vars_golden_tailoring.yml \
	%reldir%/test_inherit_selector.oval.xml \
	%reldir%/test_inherit_selector.sh \
	%reldir%/test_inherit_selector.tailoring.xml \
	%reldir%/test_inherit_selector.xccdf.xml \
	%reldir%/test_unfinished.sh \
	%reldir%/test_unfinished.xccdf.xml \
	%reldir%/test_multiple_oval_files_with_same_basename.sh \
	%reldir%/test_multiple_oval_files_with_same_basename.xccdf.xml \
	%reldir%/test_oval_without_definition.oval.xml \
	%reldir%/test_oval_without_definition.sh \
	%reldir%/test_oval_without_definition.xccdf.xml \
	%reldir%/test_profile_selection_by_suffix.sh \
	%reldir%/test_profile_selection_by_suffix.xccdf.xml \
	%reldir%/test_profile_selection_by_suffix_tailoring.xccdf.xml \
	%reldir%/test_remediate_perl.sh \
	%reldir%/test_remediate_perl.xccdf.xml \
	%reldir%/test_remediate_python.sh \
	%reldir%/test_remediate_python.xccdf.xml \
	%reldir%/test_remediate_python_subs.sh \
	%reldir%/test_remediate_python_subs.xccdf.xml \
	%reldir%/test_remediate_simple.sh \
	%reldir%/test_remediate_simple.xccdf.xml \
	%reldir%/test_remediate_unresolved.sh \
	%reldir%/test_remediate_unresolved.xccdf.xml \
	%reldir%/test_remediation_amp_escaping.sh \
	%reldir%/test_remediation_amp_escaping.xccdf.xml \
	%reldir%/test_remediation_bad_fix.sh \
	%reldir%/test_remediation_bad_fix.xccdf.xml \
	%reldir%/test_remediation_cdata.sh \
	%reldir%/test_remediation_cdata.xccdf.xml \
	%reldir%/test_remediation_fix_without_system.sh \
	%reldir%/test_remediation_fix_without_system.xccdf.xml \
	%reldir%/test_remediation_invalid_characters.sh \
	%reldir%/test_remediation_invalid_characters.xccdf.xml \
	%reldir%/test_remediation_simple.oval.xml \
	%reldir%/test_remediation_simple.sh \
	%reldir%/test_remediation_simple.xccdf.xml \
	%reldir%/test_remediation_metadata.sh \
	%reldir%/test_remediation_metadata.xccdf.xml \
	%reldir%/test_remediation_subs_plain_text.sh \
	%reldir%/test_remediation_subs_plain_text.xccdf.xml \
	%reldir%/test_remediation_subs_plain_text_empty.sh \
	%reldir%/test_remediation_subs_plain_text_empty.xccdf.xml \
	%reldir%/test_remediation_subs_unresolved.sh \
	%reldir%/test_remediation_subs_unresolved.xccdf.xml \
	%reldir%/test_remediation_subs_value_refine_value.sh \
	%reldir%/test_remediation_subs_value_refine_value.xccdf.xml \
	%reldir%/test_remediation_subs_value_take_first.sh \
	%reldir%/test_remediation_subs_value_take_first.xccdf.xml \
	%reldir%/test_remediation_subs_value_title.sh \
	%reldir%/test_remediation_subs_value_title.xccdf.xml \
	%reldir%/test_remediation_subs_value_without_selector.sh \
	%reldir%/test_remediation_subs_value_without_selector.xccdf.xml \
	%reldir%/test_remediation_xml_comments.sh \
	%reldir%/test_remediation_xml_comments.xccdf.xml \
	%reldir%/test_report_anaconda_fixes.sh \
	%reldir%/test_report_anaconda_fixes.tailoring.xml \
	%reldir%/test_report_anaconda_fixes.xccdf.xml \
	%reldir%/test_report_anaconda_fixes_ds.sh \
	%reldir%/test_report_check_with_empty_selector.oval.xml.result.xml \
	%reldir%/test_report_check_with_empty_selector.sh \
	%reldir%/test_report_check_with_empty_selector.xccdf.xml.result.xml \
	%reldir%/test_report_without_oval_poses_no_errors.sh \
	%reldir%/test_report_without_oval_poses_no_errors.xccdf.xml.result.xml \
	%reldir%/test_report_without_xsl_fails_gracefully.sh \
	%reldir%/test_single_rule.sh \
	%reldir%/test_single_rule_stigw.sh correct_stigw_result.xml stig-viewer-equivalence.py \
	%reldir%/test_single_rule.ds.xml \
	%reldir%/test_single_rule.oval.xml \
	%reldir%/test_single_rule-tailoring.xml \
	%reldir%/test_single_rule.xccdf.xml \
	%reldir%/test_xccdf_fix_attr_export.sh \
	%reldir%/test_xccdf_fix_attr_export.xccdf.xml \
	%reldir%/test_xccdf_complex_check_and_notchecked.sh \
	%reldir%/test_xccdf_complex_check_and_notchecked.xccdf.xml \
	%reldir%/test_xccdf_complex_check_nand.xccdf.xml \
	%reldir%/test_xccdf_complex_check_single_negate.xccdf.xml \
	%reldir%/test_xccdf_check_content_ref_without_name_attr.oval.xml \
	%reldir%/test_xccdf_check_content_ref_without_name_attr.sh \
	%reldir%/test_xccdf_check_content_ref_without_name_attr.xccdf.xml \
	%reldir%/test_xccdf_check_without_content_refs.sh \
	%reldir%/test_xccdf_check_without_content_refs.xccdf.xml \
	%reldir%/test_xccdf_check_negate.sh \
	%reldir%/test_xccdf_check_negate.xccdf.xml \
	%reldir%/test_xccdf_check_multi_check.sh \
	%reldir%/test_xccdf_check_multi_check.xccdf.xml \
	%reldir%/test_xccdf_check_multi_check2.sh \
	%reldir%/test_xccdf_check_multi_check2.xccdf.xml \
	%reldir%/test_xccdf_check_multi_check_zero_definitions.sh \
	%reldir%/test_xccdf_check_multi_check_zero_definitions.xccdf.xml \
	%reldir%/test_xccdf_check_processing_complex_priority.sh \
	%reldir%/test_xccdf_check_processing_complex_priority.xccdf.xml \
	%reldir%/test_xccdf_check_processing_invalid_content_refs.sh \
	%reldir%/test_xccdf_check_processing_invalid_content_refs.xccdf.xml \
	%reldir%/test_xccdf_check_processing_selector_bad.sh \
	%reldir%/test_xccdf_check_processing_selector_bad.xccdf.xml \
	%reldir%/test_xccdf_check_processing_selector_empty.sh \
	%reldir%/test_xccdf_check_processing_selector_empty.xccdf.xml \
	%reldir%/test_xccdf_check_unsupported_check_system.sh \
	%reldir%/test_xccdf_check_unsupported_check_system.xml \
	%reldir%/test_xccdf_multiple_testresults.sh \
	%reldir%/test_xccdf_multiple_testresults.xccdf.xml \
	%reldir%/test_xccdf_notchecked_has_check.sh \
	%reldir%/test_xccdf_notchecked_has_check.xccdf.xml \
	%reldir%/test_xccdf_oval_absolute_path.sh \
	%reldir%/test_xccdf_oval_absolute_path.xccdf.xml \
	%reldir%/test_xccdf_oval_relative_path.sh \
	%reldir%/test_xccdf_oval_relative_path.xccdf.xml \
	%reldir%/test_xccdf_overlaping_IDs.xccdf.xml \
	%reldir%/test_xccdf_overrides.arf.xml \
	%reldir%/test_xccdf_overrides.sh \
	%reldir%/test_xccdf_refine_rule_refine.sh \
	%reldir%/test_xccdf_refine_rule_refine.xccdf.xml \
	%reldir%/test_xccdf_refine_rule.sh \
	%reldir%/test_xccdf_refine_rule.xccdf.xml \
	%reldir%/test_xccdf_refine_value_bad.sh \
	%reldir%/test_xccdf_refine_value_bad.xccdf.xml \
	%reldir%/test_xccdf_resolve.sh \
	%reldir%/test_xccdf_resolve.xccdf.xml \
	%reldir%/test_xccdf_results_arf_no_oval.sh \
	%reldir%/test_xccdf_results_arf_no_oval.xccdf.xml \
	%reldir%/test_xccdf_role_unchecked.sh \
	%reldir%/test_xccdf_role_unchecked.xccdf.xml \
	%reldir%/test_xccdf_role_unscored.sh \
	%reldir%/test_xccdf_role_unscored.xccdf.xml \
	%reldir%/test_xccdf_selectors_cluster1.sh \
	%reldir%/test_xccdf_selectors_cluster1.xccdf.xml \
	%reldir%/test_xccdf_selectors_cluster2.sh \
	%reldir%/test_xccdf_selectors_cluster2.xccdf.xml \
	%reldir%/test_xccdf_selectors_cluster3.sh \
	%reldir%/test_xccdf_selectors_cluster3.xccdf.xml \
	%reldir%/test_xccdf_sub_title.sh \
	%reldir%/test_xccdf_sub_title.xccdf.xml \
	%reldir%/test_xccdf_test_system.sh \
	%reldir%/test_xccdf_test_system.xccdf.xml \
	%reldir%/test_xccdf_transformation.sh \
	%reldir%/test_xccdf_xml_escaping_value.sh \
	%reldir%/test_xccdf_xml_escaping_value.xccdf.xml \
	%reldir%/unit_helper.c \
	%reldir%/unit_helper.h
# HELLO PAL, THIS LIST IS ALPHABETICALLY SORTED.
