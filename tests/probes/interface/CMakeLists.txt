if(ENABLE_PROBES_UNIX)
	add_oscap_test_executable(test_probes_interface "test_probes_interface.c")
	add_oscap_test("test_probes_interface.sh")
endif()
