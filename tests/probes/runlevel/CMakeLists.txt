if(ENABLE_PROBES_UNIX)
	add_oscap_test("test_probes_runlevel.sh" LABELS unix linux_only)
	add_oscap_test("test_probes_runlevel_unsupported.sh" LABELS unix macos freebsd)
endif()
