if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpminfo.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_rpminfo_offline.sh" LABELS linux linux_only)

  set_tests_properties(
    "probes/rpm/rpminfo/test_probes_rpminfo_offline.sh"
    "probes/rpm/rpminfo/test_probes_rpminfo.sh"
    PROPERTIES RESOURCE_LOCK rpminfo
  )
endif()
