add_oscap_test_executable(test_oscap_common
	"test_oscap_common.c"
)

add_oscap_test_executable(test_xccdf_overrides
	"test_xccdf_overrides.c"
)

add_oscap_test_executable(test_xccdf_shall_pass
	test_xccdf_shall_pass.c
	unit_helper.c
)

add_oscap_test("all.sh")

if(PYTHONINTERP_FOUND)
	add_oscap_test("all_python.sh")
endif()
