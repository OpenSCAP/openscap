if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpmverify_not_equals_operation.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_rpmverify_not_equals_operation_offline.sh" LABELS linux linux_only)

  set_tests_properties(
    "probes/rpm/rpmverify/test_probes_rpmverify_not_equals_operation_offline.sh"
    "probes/rpm/rpmverify/test_probes_rpmverify_not_equals_operation.sh"
    PROPERTIES RESOURCE_LOCK rpm_shared
  )
endif()
