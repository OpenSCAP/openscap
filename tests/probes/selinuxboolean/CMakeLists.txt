if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_selinuxboolean.sh")
	add_oscap_test("test_probes_selinuxboolean_offline_mode.sh")
endif()
