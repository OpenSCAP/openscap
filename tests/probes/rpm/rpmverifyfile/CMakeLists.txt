if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpmverifyfile.sh")
	add_oscap_test("test_probes_rpmverifyfile_older.sh")
	add_oscap_test("test_probes_rpmverifyfile_offline.sh")

  set_tests_properties(
    "probes/rpm/rpmverifyfile/test_probes_rpmverifyfile_offline.sh"
    "probes/rpm/rpmverifyfile/test_probes_rpmverifyfile_older.sh"
    "probes/rpm/rpmverifyfile/test_probes_rpmverifyfile.sh"
    PROPERTIES RESOURCE_LOCK rpm_shared
  )
endif()
