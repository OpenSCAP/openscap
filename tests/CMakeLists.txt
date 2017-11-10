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

# builds a binary from a C source that will be used in a test
# EXECUTABLE_NAME - name of the binary executable to be build
# SOURCE_FILE - C program with a test
function(add_oscap_test_executable EXECUTABLE_NAME SOURCE_FILE)
	set(TEST_EXECUTABLE_LIBRARIES openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
	add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE} ${ARGN})
	target_link_libraries(${EXECUTABLE_NAME} ${TEST_EXECUTABLE_LIBRARIES})
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
