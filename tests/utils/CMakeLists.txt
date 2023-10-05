add_oscap_test("autotailor_integration_test.sh")
add_oscap_test("test_utils_args.sh")

add_test(
    NAME "autotailor-unit-tests"
    COMMAND ${PYTHON_EXECUTABLE} -m pytest -v "${CMAKE_CURRENT_SOURCE_DIR}/test_autotailor.py"
)
