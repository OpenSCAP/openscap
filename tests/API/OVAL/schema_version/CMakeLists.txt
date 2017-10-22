add_executable(test_schema_version test_schema_version.c)
set(TEST_API_OVAL_LIBRARIES openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
target_link_libraries(test_schema_version ${TEST_API_OVAL_LIBRARIES})
add_oscap_test("test_schema_version.sh")
