if(ENABLE_PROBES_INDEPENDENT)
	add_oscap_test("test_probes_filehash58.sh" LABELS independent)
	add_oscap_test("rhbz1959570_segfault.sh" LABELS independent)
endif()
