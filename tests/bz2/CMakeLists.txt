if(BZIP2_FOUND)
	add_oscap_test_executable(test_bz2_memory_source "test_bz2_memory_source.c")

	add_oscap_test("all.sh")
endif()
