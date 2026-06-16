add_oscap_test("test_xccdf_cpe_eval.sh")
add_oscap_test("test_xccdf_cpe2_eval.sh")
add_oscap_test("test_xccdf_embedded_cpe_eval.sh")
add_oscap_test("test_platform_element.sh")
add_oscap_test("test_platform_inheritance.sh")
add_oscap_test("test_remediate_fix_notapplicable.sh")
add_oscap_test("test_remediate_fix_processing.sh")
add_oscap_test("test_remediate_fix_processing_ds.sh")
add_oscap_test("test_report_anaconda_fixes.sh")

set_tests_properties(
    "API/XCCDF/applicability/test_remediate_fix_processing_ds.sh"
    "API/XCCDF/applicability/test_remediate_fix_processing.sh"
    "API/XCCDF/applicability/test_remediate_fix_notapplicable.sh"
    PROPERTIES RESOURCE_LOCK test_file_lock
)
