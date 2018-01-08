add_oscap_test("test_ds.sh")
add_oscap_test("test_rds.sh")
add_oscap_test("test_sds_compose_split.sh")
add_oscap_test("test_sds_fix_from_results.sh")
add_oscap_test("test_sds_fix_from_source.sh")

add_subdirectory("ds_sds_index")
add_subdirectory("signed")
add_subdirectory("validate")
