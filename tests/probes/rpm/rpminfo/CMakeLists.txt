if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpminfo.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_rpminfo_offline.sh" LABELS linux linux_only)
endif()
