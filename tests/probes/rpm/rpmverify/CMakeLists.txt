if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpmverify_not_equals_operation.sh")
	add_oscap_test("test_probes_rpmverify_not_equals_operation_offline.sh")
endif()
