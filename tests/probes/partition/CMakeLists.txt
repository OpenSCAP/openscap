if(ENABLE_PROBES_LINUX)
	add_oscap_test("test_probes_partition.sh")
	add_oscap_test("test_probes_partition_offline_mode.sh")
endif()
