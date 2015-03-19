#!/bin/bash

set -e -o pipefail

. $srcdir/../../../test_common.sh

test_init test_api_xccdf_unittests.log
#
# API C Tests
#
test_run "xccdf:complex-check -- NAND is working properly" ./test_xccdf_shall_pass $srcdir/test_xccdf_complex_check_nand.xccdf.xml
test_run "xccdf:complex-check -- single negation" ./test_xccdf_shall_pass $srcdir/test_xccdf_complex_check_single_negate.xccdf.xml
test_run "Certain id's of xccdf_items may overlap" ./test_xccdf_shall_pass $srcdir/test_xccdf_overlaping_IDs.xccdf.xml
test_run "Test Abstract data types." ./test_oscap_common

test_run "Assert for environment" [ ! -x $srcdir/not_executable ]
test_run "Assert for environment better" $OSCAP oval eval --id oval:moc.elpmaxe.www:def:1 $srcdir/test_xccdf_check_content_ref_without_name_attr.oval.xml

#
# General XCCDF Tests. (Mostly, oscap xccdf eval)
#
test_run "Fix containing unresolved elements" $srcdir/test_remediate_unresolved.sh
test_run "Empty XCCDF variable element" $srcdir/test_empty_variable.sh
test_run "Test xccdf:fix/xccdf:instance elements" $srcdir/test_fix_instance.sh
test_run "Escaping of xml &amp within xccdf:value" $srcdir/test_xccdf_xml_escaping_value.sh
test_run "check/@negate" $srcdir/test_xccdf_check_negate.sh
test_run "check/@multi-check import/export" $srcdir/test_xccdf_check_multi_check.sh
test_run "check/@multi-check simple" $srcdir/test_xccdf_check_multi_check2.sh
test_run "check/@multi-check that has zero definitions" $srcdir/test_xccdf_check_multi_check_zero_definitions.sh
test_run "xccdf:check-content-ref without @name" $srcdir/test_xccdf_check_content_ref_without_name_attr.sh
test_run "without xccdf:check-content-refs" $srcdir/test_xccdf_check_without_content_refs.sh
test_run "xccdf:refine-rule/@weight shall not be exported" $srcdir/test_xccdf_refine_rule.sh
test_run "xccdf:fix/@distruption|@complexity shall not be exported" $srcdir/test_xccdf_fix_attr_export.sh
test_run "xccdf:complex-check/@operator=AND -- notchecked" $srcdir/test_xccdf_complex_check_and_notchecked.sh
test_run "Check Processing Algorithm -- complex-check priority" $srcdir/test_xccdf_check_processing_complex_priority.sh
test_run "Check Processing Algorithm -- bad refine must select check without @selector" $srcdir/test_xccdf_check_processing_selector_bad.sh
test_run "Check Processing Algorithm -- none selected for candidate" $srcdir/test_xccdf_check_processing_selector_empty.sh
test_run "Check Processing Algorithm -- none check-content-ref resolvable." $srcdir/test_xccdf_check_processing_invalid_content_refs.sh
test_run "Check Processing Algorithm -- always include xccdf:check" $srcdir/test_xccdf_notchecked_has_check.sh
test_run "xccdf:select and @cluster-id -- disable group" $srcdir/test_xccdf_selectors_cluster1.sh
test_run "xccdf:select and @cluster-id -- enable a set of items" $srcdir/test_xccdf_selectors_cluster2.sh
test_run "xccdf:select and @cluster-id -- complex example" $srcdir/test_xccdf_selectors_cluster3.sh
test_run "Deriving XCCDF Check Results from OVAL Definition Results" $srcdir/test_deriving_xccdf_result_from_oval.sh
test_run "Deriving XCCDF Check Results from OVAL Definition Results 2" $srcdir/test_deriving_xccdf_result_from_oval2.sh
test_run "Deriving XCCDF Check Results from OVAL without definition." $srcdir/test_oval_without_definition.sh
test_run "Deriving XCCDF Check Results from OVAL Definition Results + multi-check" $srcdir/test_deriving_xccdf_result_from_oval_multicheck.sh
test_run "Multiple oval files with the same basename." $srcdir/test_multiple_oval_files_with_same_basename.sh
test_run "Unsupported Check System" $srcdir/test_xccdf_check_unsupported_check_system.sh
test_run "Multiple xccdf:TestResult elements" $srcdir/test_xccdf_multiple_testresults.sh
test_run "default selector for xccdf value" $srcdir/test_default_selector.sh
test_run "inherit selector for xccdf value" $srcdir/test_inherit_selector.sh
test_run "incorrect selector for xccdf value" $srcdir/test_xccdf_refine_value_bad.sh
test_run "XCCDF Substitute within Title" $srcdir/test_xccdf_sub_title.sh

test_run "libxml errors handled correctly" $srcdir/test_unfinished.sh

#
# Tests for 'oscap xccdf eval --remediate' and substitution
#
test_run "XCCDF Remediation Simple Test" $srcdir/test_remediation_simple.sh
test_run "XCCDF Remediation Bad Fix Fails to Remedy" $srcdir/test_remediation_bad_fix.sh
test_run "XCCDF Remediation Substitute Simple plain-text" $srcdir/test_remediation_subs_plain_text.sh
test_run "XCCDF Remediation Substitute Empty plain-text" $srcdir/test_remediation_subs_plain_text_empty.sh
test_run "XCCDF Remediation Substitute Value by refine-value" $srcdir/test_remediation_subs_value_refine_value.sh
test_run "XCCDF Remediation Substitute Value by first value" $srcdir/test_remediation_subs_value_take_first.sh
test_run "XCCDF Remediation Substitute Value by empty selector" $srcdir/test_remediation_subs_value_without_selector.sh
test_run "XCCDF Remediation Substitute Value by its title" $srcdir/test_remediation_subs_value_title.sh
test_run "XCCDF Remediation &amp; decoding" $srcdir/test_remediation_amp_escaping.sh
test_run "XCCDF Remediation bypass XML Comments" $srcdir/test_remediation_xml_comments.sh
test_run "XCCDF Remediation understands <[CDATA[." $srcdir/test_remediation_cdata.sh
test_run "XCCDF Remediation Aborts on unresolved element." $srcdir/test_remediation_subs_unresolved.sh
test_run "XCCDF Remediation requires fix/@system attribute" $srcdir/test_remediation_fix_without_system.sh
#
# Tests for 'oscap xccdf remediate'
#
test_run "XCCDF Remediate" $srcdir/test_remediate_simple.sh
test_run "XCCDF Remediate + python fix" $srcdir/test_remediate_python.sh
test_run "XCCDF Remediate + python fix + xhtml:object" $srcdir/test_remediate_python_subs.sh
test_run "XCCDF Remediate + perl fix" $srcdir/test_remediate_perl.sh

#
# Tests for XCCDF report
#
test_run 'generate report: xccdf:check/@selector=""' $srcdir/test_report_check_with_empty_selector.sh
test_run "generate report: missing xsl shall not segfault" $srcdir/test_report_without_xsl_fails_gracefully.sh
test_run "generate report: avoid warnings from libxml" $srcdir/test_report_without_oval_poses_no_errors.sh
test_run "generate fix: just as the anaconda does" $srcdir/test_report_anaconda_fixes.sh
test_run "generate fix: just as the anaconda does + DataStream" $srcdir/test_report_anaconda_fixes_ds.sh
test_run "generate fix: ensure filtering drop fixes" $srcdir/test_fix_filtering.sh

test_exit
