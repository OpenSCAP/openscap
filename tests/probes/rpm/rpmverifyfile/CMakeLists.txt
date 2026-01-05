if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpmverifyfile.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_rpmverifyfile_older.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_rpmverifyfile_offline.sh" LABELS linux linux_only)

  set_tests_properties(
    "probes/rpm/rpmverifyfile/test_probes_rpmverifyfile_offline.sh"
    "probes/rpm/rpmverifyfile/test_probes_rpmverifyfile_older.sh"
    "probes/rpm/rpmverifyfile/test_probes_rpmverifyfile.sh"
    PROPERTIES RESOURCE_LOCK rpm_shared
  )
endif()
