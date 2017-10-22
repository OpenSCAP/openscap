if(BZIP2_FOUND)
	add_executable(test_bz2_memory_source "test_bz2_memory_source.c")
	target_link_libraries(test_bz2_memory_source openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES} ${BZIP2_LIBRARIES})

	add_oscap_test("all.sh")
endif()
