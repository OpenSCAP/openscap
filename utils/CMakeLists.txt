file(GLOB UTILS_SOURCES "*.c")
add_executable(oscap ${UTILS_SOURCES})
target_link_libraries(oscap openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})
