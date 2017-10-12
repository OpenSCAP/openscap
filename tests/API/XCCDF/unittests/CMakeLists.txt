add_executable(test_oscap_common
	test_oscap_common.c
	${CMAKE_SOURCE_DIR}/src/common/util.c
	${CMAKE_SOURCE_DIR}/src/common/list.c
	${CMAKE_SOURCE_DIR}/src/common/alloc.c
)
target_link_libraries(test_oscap_common openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})

add_executable(test_xccdf_overrides
	test_xccdf_overrides.c
)
target_link_libraries(test_xccdf_overrides openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})

add_executable(test_xccdf_shall_pass
	test_xccdf_shall_pass.c
	unit_helper.c
)
target_link_libraries(test_xccdf_shall_pass openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})

add_oscap_test("all.sh")
