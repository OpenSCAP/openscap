add_oscap_test_executable(test_api_cvrf "test_api_cvrf.c")
target_include_directories(test_api_cvrf PUBLIC ${CMAKE_SOURCE_DIR}/utils)
add_oscap_test("test_api_cvrf.sh")
