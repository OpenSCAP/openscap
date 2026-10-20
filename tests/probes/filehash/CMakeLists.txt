if(OPENSCAP_PROBE_INDEPENDENT_FILEHASH)
	add_oscap_test("test_probes_filehash.sh" LABELS independent)
	add_oscap_test("test_probes_filehash_dos.sh" LABELS independent)
endif()
