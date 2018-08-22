add_oscap_test_executable(test_glob_to_regex
	"test_glob_to_regex.c"
)
target_include_directories(test_glob_to_regex PRIVATE
	"${CMAKE_SOURCE_DIR}/src/OVAL"
	"${CMAKE_SOURCE_DIR}/src/common"
)
add_oscap_test("test_glob_to_regex.sh")
