if(ENABLE_PROBES_LINUX)
	if(DBUS_FOUND)
		add_oscap_ctest("probes/systemdunitproperty/test_systemd_units.sh"
			COMMAND bash "${CMAKE_CURRENT_SOURCE_DIR}/test_systemd_units.sh"
			LABELS linux linux_only)
		set_tests_properties("probes/systemdunitproperty/test_systemd_units.sh" PROPERTIES
			SKIP_RETURN_CODE 255
			ENVIRONMENT "srcdir=${CMAKE_CURRENT_SOURCE_DIR};builddir=${CMAKE_BINARY_DIR}")
		add_oscap_test("test_probes_systemdunitproperty.sh" LABELS linux linux_only)
		add_oscap_test("test_probes_systemdunitproperty_mount_wants.sh" LABELS linux linux_only)
		add_oscap_test("test_probes_systemdunitproperty_offline_mode.sh" LABELS linux linux_only)
		set_tests_properties(
			"probes/systemdunitproperty/test_probes_systemdunitproperty.sh"
			"probes/systemdunitproperty/test_probes_systemdunitproperty_mount_wants.sh"
			"probes/systemdunitproperty/test_probes_systemdunitproperty_offline_mode.sh"
			PROPERTIES RESOURCE_LOCK "systemd:1"
		)
	endif()
endif()
