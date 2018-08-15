add_oscap_test_executable(test_glob_to_regex
	"test_glob_to_regex.c"
	"${CMAKE_SOURCE_DIR}/src/OVAL/oval_glob_to_regex.c"
	"${CMAKE_SOURCE_DIR}/src/common/oscap_string.c"
	"${CMAKE_SOURCE_DIR}/src/common/oscap_buffer.c"
)
target_include_directories(test_glob_to_regex PRIVATE
	"${CMAKE_SOURCE_DIR}/src/OVAL"
	"${CMAKE_SOURCE_DIR}/src/common"
)
add_oscap_test("test_glob_to_regex.sh")
