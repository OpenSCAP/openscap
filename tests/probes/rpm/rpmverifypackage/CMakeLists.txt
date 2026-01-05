if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpmverifypackage.sh")
	add_oscap_test("test_probes_rpmverifypackage_offline.sh")

  set_tests_properties(
    "probes/rpm/rpmverifypackage/test_probes_rpmverifypackage_offline.sh"
    "probes/rpm/rpmverifypackage/test_probes_rpmverifypackage.sh"
    PROPERTIES RESOURCE_LOCK rpm_shared
  )
endif()
