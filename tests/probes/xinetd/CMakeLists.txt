if(ENABLE_PROBES_UNIX)
	add_oscap_test_executable(test_probe_xinetd
		"test_probe_xinetd.c"
		"${CMAKE_SOURCE_DIR}/src/common/bfind.c"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/rbt/rbt_common.c"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/rbt/rbt_str.c"
	)
	target_link_libraries(test_probe_xinetd openscap)
	target_include_directories(test_probe_xinetd PUBLIC
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/rbt"
		"${CMAKE_SOURCE_DIR}/src/common"
	)
	add_oscap_test("test_xinetd_probe.sh")
endif()
