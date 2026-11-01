add_oscap_test("report.sh")

find_program(NODE_EXECUTABLE NAMES node nodejs)
if(NODE_EXECUTABLE)
	add_oscap_ctest("report/sort_groups"
		COMMAND ${NODE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sort_groups.js
		LABELS "javascript"
	)
else()
	message(STATUS "Node.js not found; skipping report/sort_groups test")
endif()