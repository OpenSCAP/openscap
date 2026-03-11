if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpmverifypackage.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_rpmverifypackage_offline.sh" LABELS linux linux_only)
endif()
