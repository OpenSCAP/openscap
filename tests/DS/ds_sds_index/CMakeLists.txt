add_executable(test_ds_sds_index "test_ds_sds_index.c")
add_executable(test_ds_sds_index_multiple "test_ds_sds_index_multiple.c")
add_executable(test_ds_sds_index_invalid "test_ds_sds_index_invalid.c")
target_link_libraries(test_ds_sds_index openscap ${LIBXML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES} ${BZIP2_LIBRARIES})
target_link_libraries(test_ds_sds_index_multiple openscap ${LIBXML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES} ${BZIP2_LIBRARIES})
target_link_libraries(test_ds_sds_index_invalid openscap ${LIBXML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES} ${BZIP2_LIBRARIES})

add_oscap_test("all.sh")
