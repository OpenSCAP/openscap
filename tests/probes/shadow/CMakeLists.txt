if(ENABLE_PROBES_UNIX)
	add_oscap_test("test_probes_shadow.sh")
	add_oscap_test("test_probes_shadow_offline.sh")
endif()
