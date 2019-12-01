if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_rpminfo.sh")
	add_oscap_test("test_probes_rpminfo_offline.sh")
endif()
