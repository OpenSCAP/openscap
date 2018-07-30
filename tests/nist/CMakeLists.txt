if(PYTHONINTERP_FOUND)
	# nist test request the test_worker.py script
	add_oscap_test("nist_test.sh")
endif()
