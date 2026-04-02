if(ENABLE_PROBES_LINUX)
	if(DBUS_FOUND)
		add_oscap_test("test_probes_systemdunitdependency.sh")
		add_oscap_test("test_probes_systemdunitdependency_validation.sh")
		add_oscap_test("test_probes_systemdunitdependency_offline_mode.sh")
		set_tests_properties(
			"probes/systemdunitdependency/test_probes_systemdunitdependency.sh"
			"probes/systemdunitdependency/test_probes_systemdunitdependency_validation.sh"
			"probes/systemdunitdependency/test_probes_systemdunitdependency_offline_mode.sh"
			PROPERTIES RESOURCE_LOCK "systemd:1"
		)
	endif()
endif()
