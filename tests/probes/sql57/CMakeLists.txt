if(ENABLE_PROBES_INDEPENDENT AND TARGET sql57_probe)
	add_oscap_test("unsupported_engine.sh")
endif()
