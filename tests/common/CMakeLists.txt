add_oscap_test_executable(test_oscap_util
	"test_oscap_util.c"
	${CMAKE_SOURCE_DIR}/src/common/util.c
	${CMAKE_SOURCE_DIR}/src/common/error.c
	${CMAKE_SOURCE_DIR}/src/common/err_queue.c
)

add_oscap_test("test_oscap_util.sh")

add_oscap_test_executable(test_verbose_error "test_verbose_error.c")
add_oscap_ctest("common/test_verbose_error" COMMAND test_verbose_error)

if(NOT WIN32)
	add_oscap_test_executable(test_tmpdir "test_tmpdir.c")
	add_oscap_test("test_tmpdir.sh")
endif()
