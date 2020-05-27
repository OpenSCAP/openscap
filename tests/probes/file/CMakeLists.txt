if(ENABLE_PROBES_UNIX)
	add_oscap_test("test_probes_file.sh")
	add_oscap_test("test_probes_file_multiple_file_paths.sh")
endif()
