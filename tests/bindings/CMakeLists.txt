if(ENABLE_PERL)
	# this test tests nothing
	add_oscap_test("test_perl.sh")
endif()
if(ENABLE_PYTHON2)
	add_oscap_test("test_python.sh")
endif()
