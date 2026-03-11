if(ENABLE_PROBES_UNIX)
	add_oscap_test("test_probes_symlink.sh" LABELS unix)
	add_oscap_test("test_offline_mode_symlink.sh" LABELS unix)
endif()
