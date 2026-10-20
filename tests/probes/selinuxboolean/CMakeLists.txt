if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_selinuxboolean.sh" LABELS linux linux_only)
	add_oscap_test("test_probes_selinuxboolean_offline_mode.sh" LABELS linux linux_only)
endif()
