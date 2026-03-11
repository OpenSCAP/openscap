if(ENABLE_PROBES_UNIX)
	add_oscap_test("test_probes_runlevel.sh" LABELS unix linux_only)
endif()
