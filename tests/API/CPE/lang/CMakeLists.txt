add_executable(test_api_cpe_lang "test_api_cpe_lang.c")
target_link_libraries(test_api_cpe_lang openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
add_oscap_test("test_api_cpe_lang.sh")
