if(ENABLE_PROBES_INDEPENDENT)
	add_oscap_test_executable(test_probes_sysinfo "test_probes_sysinfo.c")
	add_oscap_test("test_probes_sysinfo.sh")
endif()
