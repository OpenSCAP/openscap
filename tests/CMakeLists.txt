function(add_oscap_test TEST_SCRIPT)
	add_test(NAME ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT}
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT}
	)
	set_tests_properties(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT}
		PROPERTIES
			SKIP_RETURN_CODE 255
			ENVIRONMENT "srcdir=${CMAKE_CURRENT_SOURCE_DIR};top_srcdir=${CMAKE_SOURCE_DIR};builddir=${CMAKE_BINARY_DIR}"
	)
endfunction()

configure_file("test_common.sh.in" "test_common.sh" @ONLY)

add_subdirectory("API")
add_subdirectory("bindings")
add_subdirectory("bz2")
add_subdirectory("codestyle")
add_subdirectory("CPE")
add_subdirectory("DS")
add_subdirectory("mitre")
add_subdirectory("nist")
add_subdirectory("oscap_string")
add_subdirectory("oval_details")
add_subdirectory("probes")
add_subdirectory("sce")
add_subdirectory("schemas")
add_subdirectory("sources")
