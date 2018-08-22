add_oscap_test_executable(test_api_probes_smoke
	"test_api_probes_smoke.c"
)
target_include_directories(test_api_probes_smoke PUBLIC
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
	"${CMAKE_SOURCE_DIR}/src/common"
)

add_oscap_test_executable(oval_fts_list
	"oval_fts_list.c"
)
target_include_directories(oval_fts_list PUBLIC
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
	"${CMAKE_SOURCE_DIR}/src/OVAL/results"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/probe"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
	"${CMAKE_SOURCE_DIR}/src/common"
)

add_oscap_test("all.sh")
