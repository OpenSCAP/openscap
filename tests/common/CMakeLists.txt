add_oscap_test_executable(test_oscap_util
	"test_oscap_util.c"
	${CMAKE_SOURCE_DIR}/src/common/util.c
	${CMAKE_SOURCE_DIR}/src/common/error.c
	${CMAKE_SOURCE_DIR}/src/common/err_queue.c
)

add_oscap_test("test_oscap_util.sh")

if(NOT WIN32)
	add_oscap_test_executable(test_tmpdir "test_tmpdir.c")
	add_oscap_test("test_tmpdir.sh")
endif()
