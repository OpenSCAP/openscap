function(add_oscap_test test_script)
	add_test(${CMAKE_CURRENT_SOURCE_DIR}/${test_script} ${CMAKE_CURRENT_SOURCE_DIR}/${test_script})
	set_tests_properties(${CMAKE_CURRENT_SOURCE_DIR}/${test_script}
		PROPERTIES
			SKIP_RETURN_CODE 255
			ENVIRONMENT "srcdir=${CMAKE_CURRENT_SOURCE_DIR};top_srcdir=${CMAKE_SOURCE_DIR};builddir=${CMAKE_BINARY_DIR}"
	)
endfunction()

configure_file("test_common.sh.in" "test_common.sh" @ONLY)
