if(ENABLE_PROBES_LINUX)
	if(DBUS_FOUND)
		add_oscap_test("test_probes_fwupdsecattr.sh")
		add_oscap_test("test_probes_fwupdsecattr_mock.sh")
	endif()
endif()

