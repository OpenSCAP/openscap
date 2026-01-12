if(ENABLE_PROBES_LINUX)
	if(DBUS_FOUND)
		add_oscap_test("test_probes_systemdunitproperty.sh")
		add_oscap_test("test_probes_systemdunitproperty_mount_wants.sh")
		add_oscap_test("test_probes_systemdunitproperty_offline_mode.sh")
		set_tests_properties(
			"probes/systemdunitproperty/test_probes_systemdunitproperty.sh"
			"probes/systemdunitproperty/test_probes_systemdunitproperty_mount_wants.sh"
			"probes/systemdunitproperty/test_probes_systemdunitproperty_offline_mode.sh"
			PROPERTIES RESOURCE_LOCK "systemd:1"
		)
	endif()
endif()
