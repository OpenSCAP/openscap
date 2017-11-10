add_oscap_test_executable(test_api_oval "test_api_oval.c")
add_oscap_test_executable(test_api_syschar "test_api_syschar.c")
add_oscap_test_executable(test_api_results "test_api_results.c")
add_oscap_test_executable(test_api_directives "test_api_directives.c")

add_oscap_test("test_api_oval.sh")

add_subdirectory("glob_to_regex")
add_subdirectory("report_variable_values")
add_subdirectory("schema_version")
add_subdirectory("unittests")
add_subdirectory("validate")
