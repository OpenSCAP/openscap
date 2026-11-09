if(OPENSCAP_PROBE_LINUX_DPKGINFO)
	add_oscap_test("test_probes_dpkginfo.sh" LABELS linux linux_only)
endif()
