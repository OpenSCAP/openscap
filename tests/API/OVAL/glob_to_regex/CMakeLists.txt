add_executable(test_glob_to_regex test_glob_to_regex.c)
set(TEST_API_OVAL_LIBRARIES openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
target_link_libraries(test_glob_to_regex ${TEST_API_OVAL_LIBRARIES})
add_oscap_test("test_glob_to_regex.sh")
