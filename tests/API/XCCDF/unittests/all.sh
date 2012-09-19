#!/bin/bash


. $srcdir/../../../test_common.sh

test_init test_api_xccdf_unittests.log
test_run "xccdf:complex-check -- NAND is working properly" ./test_xccdf_shall_pass $srcdir/test_xccdf_complex_check_nand.xccdf.xml
test_run "xccdf:complex-check -- single negation" ./test_xccdf_shall_pass $srcdir/test_xccdf_complex_check_single_negate.xccdf.xml
test_run "Certain id's of xccdf_items may overlap" ./test_xccdf_shall_pass $srcdir/test_xccdf_overlaping_IDs.xccdf.xml
test_run "Test Abstract data types." ./test_oscap_common

test_run "Assert for environment" [ ! -x $srcdir/not_executable ]
test_run "Assert for environment better" $OSCAP oval eval --id oval:moc.elpmaxe.www:def:1 $srcdir/test_xccdf_check_content_ref_without_name_attr.oval.xml

test_run "Escaping of xml &amp within xccdf:value" $srcdir/test_xccdf_xml_escaping_value.sh
test_run "check/@negate" $srcdir/test_xccdf_check_negate.sh
test_run "check/@multi-check" $srcdir/test_xccdf_check_multi_check.sh
test_run "xccdf:check-content-ref without @name" $srcdir/test_xccdf_check_content_ref_without_name_attr.sh
test_run "xccdf:refine-rule/@weight shall not be exported" $srcdir/test_xccdf_refine_rule.sh
test_run "Check Processing Algorithm -- complex-check priority" $srcdir/test_xccdf_check_processing_complex_priority.sh
test_run "Check Processing Algorithm -- bad refine must select check without @selector" $srcdir/test_xccdf_check_processing_selector_bad.sh
test_exit
