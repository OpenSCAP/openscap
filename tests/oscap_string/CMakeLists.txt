add_executable(test_oscap_string
	"test_oscap_string.c"
	${CMAKE_SOURCE_DIR}/src/common/oscap_string.c
	${CMAKE_SOURCE_DIR}/src/common/oscap_buffer.c
	${CMAKE_SOURCE_DIR}/src/common/alloc.c
)
target_link_libraries(test_oscap_string openscap ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES} ${BZIP2_LIBRARIES})

add_oscap_test("test_oscap_string.sh")
