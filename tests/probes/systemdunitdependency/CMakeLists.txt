if(ENABLE_PROBES_LINUX)
	if(DBUS_FOUND)
		add_oscap_test("test_probes_systemdunitdependency.sh" LABELS linux linux_only)
		add_oscap_test("test_probes_systemdunitdependency_validation.sh" LABELS linux linux_only)
		add_oscap_test("test_probes_systemdunitdependency_offline_mode.sh" LABELS linux linux_only)
		set_tests_properties(
			"probes/systemdunitdependency/test_probes_systemdunitdependency.sh"
			"probes/systemdunitdependency/test_probes_systemdunitdependency_validation.sh"
			"probes/systemdunitdependency/test_probes_systemdunitdependency_offline_mode.sh"
			PROPERTIES RESOURCE_LOCK "systemd:1"
		)
	endif()
endif()
