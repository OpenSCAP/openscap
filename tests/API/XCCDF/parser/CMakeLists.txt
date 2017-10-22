add_executable(test_api_xccdf test_api_xccdf.c)
target_link_libraries(test_api_xccdf openscap ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${RPM_LIBRARIES})
add_oscap_test("test_api_xccdf.sh")
