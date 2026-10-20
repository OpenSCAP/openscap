if(ENABLE_PROBES_INDEPENDENT)
	add_oscap_test("test_probes_yamlfilecontent_key.sh" LABELS independent)
	add_oscap_test("test_probes_yamlfilecontent_array.sh" LABELS independent)
	add_oscap_test("test_probes_yamlfilecontent_offline_mode.sh" LABELS independent)
	add_oscap_test("test_probes_yamlfilecontent_types.sh" LABELS independent)
	add_oscap_test("test_probes_yamlfilecontent_content.sh" LABELS independent)

	set_tests_properties(
		"probes/yamlfilecontent/test_probes_yamlfilecontent_array.sh"
		"probes/yamlfilecontent/test_probes_yamlfilecontent_offline_mode.sh"
		PROPERTIES RESOURCE_LOCK tmp_openshift_logging_yaml
	)
endif()
