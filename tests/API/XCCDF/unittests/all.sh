#!/bin/bash

set -e -o pipefail

test_srcdir="$srcdir/unittests"
srcdir="$test_srcdir"

builddir="${builddir:-../..}"

. "$builddir/tests/test_common.sh"

test_init test_api_xccdf_unittests.log

#
# API C Tests
#
if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "xccdf:complex-check -- NAND is working properly" ./test_xccdf_shall_pass $test_srcdir/test_xccdf_complex_check_nand.xccdf.xml
    test_run "xccdf:complex-check -- single negation" ./test_xccdf_shall_pass $test_srcdir/test_xccdf_complex_check_single_negate.xccdf.xml
    test_run "Certain id's of xccdf_items may overlap" ./test_xccdf_shall_pass $test_srcdir/test_xccdf_overlaping_IDs.xccdf.xml
    test_run "Test Abstract data types." ./test_oscap_common
    test_run "xccdf_rule_result_override" $test_srcdir/test_xccdf_overrides.sh

    test_run "Assert for environment" [ ! -x $test_srcdir/not_executable ]
    test_run "Assert for environment better" $OSCAP oval eval --id oval:moc.elpmaxe.www:def:1 $test_srcdir/test_xccdf_check_content_ref_without_name_attr.oval.xml
fi

#
# General XCCDF Tests. (Mostly, oscap xccdf eval)
#
test_run "Test unscored roles" $test_srcdir/test_xccdf_role_unscored.sh
test_run "Fix containing unresolved elements" $test_srcdir/test_remediate_unresolved.sh
test_run "Empty XCCDF variable element" $test_srcdir/test_empty_variable.sh
test_run "Test xccdf:fix/xccdf:instance elements" $test_srcdir/test_fix_instance.sh
test_run "Escaping of xml &amp within xccdf:value" $test_srcdir/test_xccdf_xml_escaping_value.sh
test_run "check/@negate" $test_srcdir/test_xccdf_check_negate.sh
test_run "check/@multi-check import/export" $test_srcdir/test_xccdf_check_multi_check.sh
test_run "check/@multi-check simple" $test_srcdir/test_xccdf_check_multi_check2.sh
test_run "check/@multi-check that has zero definitions" $test_srcdir/test_xccdf_check_multi_check_zero_definitions.sh
test_run "xccdf:check-content-ref without @name" $test_srcdir/test_xccdf_check_content_ref_without_name_attr.sh
test_run "without xccdf:check-content-refs" $test_srcdir/test_xccdf_check_without_content_refs.sh
test_run "xccdf:refine-rule/@weight shall not be exported" $test_srcdir/test_xccdf_refine_rule.sh
test_run "xccdf:refine-rule shall refine rules" $test_srcdir/test_xccdf_refine_rule_refine.sh
test_run "xccdf:fix/@distruption|@complexity shall not be exported" $test_srcdir/test_xccdf_fix_attr_export.sh
test_run "xccdf:complex-check/@operator=AND -- notchecked" $test_srcdir/test_xccdf_complex_check_and_notchecked.sh
test_run "Check Processing Algorithm -- complex-check priority" $test_srcdir/test_xccdf_check_processing_complex_priority.sh
test_run "Check Processing Algorithm -- bad refine must select check without @selector" $test_srcdir/test_xccdf_check_processing_selector_bad.sh
test_run "Check Processing Algorithm -- none selected for candidate" $test_srcdir/test_xccdf_check_processing_selector_empty.sh
test_run "Check Processing Algorithm -- none check-content-ref resolvable." $test_srcdir/test_xccdf_check_processing_invalid_content_refs.sh
test_run "Check Processing Algorithm -- don't include xccdf:check if result is notchecked" $test_srcdir/test_xccdf_notchecked_has_check.sh
test_run "Check Processing Algorithm -- notchecked & unselected" $test_srcdir/test_xccdf_role_unchecked.sh
test_run "Load OVAL using absolute path" $test_srcdir/test_xccdf_oval_absolute_path.sh
test_run "Load OVAL using relative path" $test_srcdir/test_xccdf_oval_relative_path.sh
test_run "xccdf:select and @cluster-id -- disable group" $test_srcdir/test_xccdf_selectors_cluster1.sh
test_run "xccdf:select and @cluster-id -- enable a set of items" $test_srcdir/test_xccdf_selectors_cluster2.sh
test_run "xccdf:select and @cluster-id -- complex example" $test_srcdir/test_xccdf_selectors_cluster3.sh
test_run "Deriving XCCDF Check Results from OVAL Definition Results" $test_srcdir/test_deriving_xccdf_result_from_oval.sh
test_run "Deriving XCCDF Check Results from OVAL Definition Results 2" $test_srcdir/test_deriving_xccdf_result_from_oval2.sh
test_run "Deriving XCCDF Check Results from OVAL without definition." $test_srcdir/test_oval_without_definition.sh
test_run "Deriving XCCDF Check Results from OVAL Definition Results + multi-check" $test_srcdir/test_deriving_xccdf_result_from_oval_multicheck.sh
test_run "Multiple oval files with the same basename." $test_srcdir/test_multiple_oval_files_with_same_basename.sh
test_run "Unsupported Check System" $test_srcdir/test_xccdf_check_unsupported_check_system.sh
test_run "Multiple xccdf:TestResult elements" $test_srcdir/test_xccdf_multiple_testresults.sh
test_run "default selector for xccdf value" $test_srcdir/test_default_selector.sh
test_run "inherit selector for xccdf value" $test_srcdir/test_inherit_selector.sh
test_run "incorrect selector for xccdf value" $test_srcdir/test_xccdf_refine_value_bad.sh
test_run "test xccdf resolve" $test_srcdir/test_xccdf_resolve.sh
test_run "Exported arf results from xccdf without reference to oval" $test_srcdir/test_xccdf_results_arf_no_oval.sh
test_run "XCCDF Substitute within Title" $test_srcdir/test_xccdf_sub_title.sh
test_run "TestResult element should contain test-system attribute" $test_srcdir/test_xccdf_test_system.sh
test_run "Profile suffix matching" $test_srcdir/test_profile_selection_by_suffix.sh

test_run "libxml errors handled correctly" $test_srcdir/test_unfinished.sh
test_run "XCCDF 1.1 to 1.2 transformation" $test_srcdir/test_xccdf_transformation.sh
test_run "Test single-rule evaluation" $test_srcdir/test_single_rule.sh
test_run "Test STIG viewer output for single-rule evaluation" $test_srcdir/test_single_rule_stigw.sh

#
# Tests for 'oscap xccdf eval --remediate' and substitution
#
test_run "XCCDF Remediation Simple Test" $test_srcdir/test_remediation_simple.sh
test_run "XCCDF Remediation Contains Metadata Test" $test_srcdir/test_remediation_metadata.sh
test_run "XCCDF Remediation Bad Fix Fails to Remedy" $test_srcdir/test_remediation_bad_fix.sh
test_run "XCCDF Remediation Substitute Simple plain-text" $test_srcdir/test_remediation_subs_plain_text.sh
test_run "XCCDF Remediation Substitute Empty plain-text" $test_srcdir/test_remediation_subs_plain_text_empty.sh
test_run "XCCDF Remediation Substitute Value by refine-value" $test_srcdir/test_remediation_subs_value_refine_value.sh
test_run "XCCDF Remediation Substitute Value by first value" $test_srcdir/test_remediation_subs_value_take_first.sh
test_run "XCCDF Remediation Substitute Value by empty selector" $test_srcdir/test_remediation_subs_value_without_selector.sh
test_run "XCCDF Remediation Substitute Value by its title" $test_srcdir/test_remediation_subs_value_title.sh
test_run "XCCDF Remediation &amp; decoding" $test_srcdir/test_remediation_amp_escaping.sh
test_run "XCCDF Remediation bypass XML Comments" $test_srcdir/test_remediation_xml_comments.sh
test_run "XCCDF Remediation understands <[CDATA[." $test_srcdir/test_remediation_cdata.sh
test_run "XCCDF Remediation Aborts on unresolved element." $test_srcdir/test_remediation_subs_unresolved.sh
test_run "XCCDF Remediation requires fix/@system attribute" $test_srcdir/test_remediation_fix_without_system.sh
test_run "XCCDF Remediation output should not contain unallowed characters" $test_srcdir/test_remediation_invalid_characters.sh
#
# Tests for 'oscap xccdf remediate'
#
test_run "XCCDF Remediate" $test_srcdir/test_remediate_simple.sh
test_run "XCCDF Remediate + python fix" $test_srcdir/test_remediate_python.sh
test_run "XCCDF Remediate + python fix + xhtml:object" $test_srcdir/test_remediate_python_subs.sh
test_run "XCCDF Remediate + perl fix" $test_srcdir/test_remediate_perl.sh

#
# Tests for XCCDF report
#
test_run 'generate report: xccdf:check/@selector=""' $test_srcdir/test_report_check_with_empty_selector.sh
test_run "generate report: missing xsl shall not segfault" $test_srcdir/test_report_without_xsl_fails_gracefully.sh
test_run "generate report: avoid warnings from libxml" $test_srcdir/test_report_without_oval_poses_no_errors.sh

#
# Tests for 'oscap xccdf generate fix'
#
test_run "generate fix: just as the anaconda does" $test_srcdir/test_report_anaconda_fixes.sh
test_run "generate fix: just as the anaconda does + DataStream" $test_srcdir/test_report_anaconda_fixes_ds.sh
test_run "generate fix: ensure filtering drop fixes" $test_srcdir/test_fix_filtering.sh
test_run "generate fix: generate header for bash script" $test_srcdir/test_fix_script_header.sh
test_run "generate fix: from result DataStream" $test_srcdir/test_fix_arf.sh
test_run "generate fix: result id selection by suffix" $test_srcdir/test_fix_resultid_by_suffix.sh
test_run "generate fix: from result DataStream" $test_srcdir/test_fix_arf.sh
test_run "generate fix: Ansible variables sanity" $test_srcdir/test_generate_fix_ansible_vars.sh

test_exit
