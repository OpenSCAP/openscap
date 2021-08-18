if(ENABLE_PROBES_UNIX)
	add_oscap_test("test_probes_symlink.sh")
	add_oscap_test("test_offline_mode_symlink.sh")
endif()
