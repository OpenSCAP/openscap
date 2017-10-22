add_executable(test_api_cvss "test_api_cvss.c")
target_link_libraries(test_api_cvss openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
add_oscap_test("test_api_cvss.sh")
