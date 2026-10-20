if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_iflisteners.sh" LABELS linux linux_only)
endif()
