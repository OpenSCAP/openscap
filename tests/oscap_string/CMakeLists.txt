add_oscap_test_executable(test_oscap_string
	"test_oscap_string.c"
	# some of the tested functions are private symbols from the following files
	${CMAKE_SOURCE_DIR}/src/common/oscap_string.c
	${CMAKE_SOURCE_DIR}/src/common/oscap_buffer.c
	${CMAKE_SOURCE_DIR}/src/common/alloc.c
)

add_oscap_test("test_oscap_string.sh")
