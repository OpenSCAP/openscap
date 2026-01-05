if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpminfo.sh")
	add_oscap_test("test_probes_rpminfo_offline.sh")

  set_tests_properties(
    "probes/rpm/rpminfo/test_probes_rpminfo_offline.sh"
    "probes/rpm/rpminfo/test_probes_rpminfo.sh"
    PROPERTIES RESOURCE_LOCK rpm_shared
  )
endif()
