add_oscap_test_executable(test_oscap_common
	"test_oscap_common.c"
	${CMAKE_SOURCE_DIR}/src/common/util.c
	${CMAKE_SOURCE_DIR}/src/common/list.c
)

add_oscap_test_executable(test_xccdf_overrides
	"test_xccdf_overrides.c"
)

add_oscap_test_executable(test_xccdf_shall_pass
	test_xccdf_shall_pass.c
	unit_helper.c
)
target_link_libraries(test_xccdf_shall_pass openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})

add_oscap_test("all.sh")

if(PYTHON_EXECUTABLE)
	add_oscap_test("all_python.sh")
endif()
