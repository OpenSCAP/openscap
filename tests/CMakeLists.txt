include(CMakeParseArguments)

function(add_oscap_test TEST_SCRIPT)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs LABELS)
	cmake_parse_arguments(OSCAP_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	string(REPLACE "${CMAKE_SOURCE_DIR}/tests/" "" TEST_NAME "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT}")
	add_test(NAME ${TEST_NAME}
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT}
	)
	set(TEST_LABELS "shell")
	# Add a suite label derived from the first path component (api, probes, report, ...)
	string(REGEX MATCH "^[^/]+" TEST_SUITE "${TEST_NAME}")
	if(TEST_SUITE)
		string(TOLOWER "${TEST_SUITE}" TEST_SUITE_LOWER)
		list(APPEND TEST_LABELS "${TEST_SUITE_LOWER}")
	endif()
	if(OSCAP_TEST_LABELS)
		list(APPEND TEST_LABELS ${OSCAP_TEST_LABELS})
	endif()
	list(REMOVE_DUPLICATES TEST_LABELS)
	set_tests_properties(${TEST_NAME}
		PROPERTIES
			SKIP_RETURN_CODE 255
			LABELS "${TEST_LABELS}"
			ENVIRONMENT "srcdir=${CMAKE_CURRENT_SOURCE_DIR};top_srcdir=${CMAKE_SOURCE_DIR};builddir=${CMAKE_BINARY_DIR}"
	)
endfunction()

function(add_oscap_ctest TEST_NAME)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs COMMAND LABELS)
	cmake_parse_arguments(OSCAP_CTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT OSCAP_CTEST_COMMAND)
		message(FATAL_ERROR "add_oscap_ctest requires COMMAND arguments")
	endif()

	add_test(NAME ${TEST_NAME}
		COMMAND ${OSCAP_CTEST_COMMAND}
	)

	set(TEST_LABELS "ctest")
	string(REPLACE "${CMAKE_SOURCE_DIR}/tests/" "" TEST_REL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
	string(REGEX MATCH "^[^/]+" TEST_SUITE "${TEST_REL_DIR}")
	if(TEST_SUITE)
		string(TOLOWER "${TEST_SUITE}" TEST_SUITE_LOWER)
		list(APPEND TEST_LABELS "${TEST_SUITE_LOWER}")
	endif()
	if(OSCAP_CTEST_LABELS)
		list(APPEND TEST_LABELS ${OSCAP_CTEST_LABELS})
	endif()
	list(REMOVE_DUPLICATES TEST_LABELS)

	set_tests_properties(${TEST_NAME}
		PROPERTIES
			LABELS "${TEST_LABELS}"
	)
endfunction()

# builds a binary from a C source that will be used in a test
# EXECUTABLE_NAME - name of the binary executable to be build
# SOURCE_FILE - C program with a test
function(add_oscap_test_executable EXECUTABLE_NAME SOURCE_FILE)
	set(TEST_EXECUTABLE_LIBRARIES openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE2_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
	add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE} ${ARGN})
	target_link_libraries(${EXECUTABLE_NAME} ${TEST_EXECUTABLE_LIBRARIES})
	target_include_directories(${EXECUTABLE_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/tests")
endfunction()

configure_file("test_common.sh.in" "test_common.sh" @ONLY)

add_subdirectory("API")
add_subdirectory("bindings")
add_subdirectory("bz2")
add_subdirectory("codestyle")
add_subdirectory("curl")
add_subdirectory("CPE")
add_subdirectory("DS")
add_subdirectory("mitre")
add_subdirectory("nist")
add_subdirectory("oscap_info_profiles")
add_subdirectory("oscap_string")
add_subdirectory("oval_details")
add_subdirectory("probe_behavior")
add_subdirectory("probes")
add_subdirectory("report")
add_subdirectory("sce")
add_subdirectory("schemas")
add_subdirectory("sources")
add_subdirectory("utils")
add_subdirectory("common")
add_subdirectory("show_details")
