add_oscap_test_executable(test_api_probes_smoke
	"test_api_probes_smoke.c"
	"${CMAKE_SOURCE_DIR}/src/common/memusage.c"
	"${CMAKE_SOURCE_DIR}/src/common/bfind.c"
)
target_include_directories(test_api_probes_smoke PUBLIC
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
	"${CMAKE_SOURCE_DIR}/src/common"
)
add_oscap_test("test_api_probes_smoke.sh")

add_oscap_test_executable(test_fsdev_is_local_fs
	"test_fsdev_is_local_fs.c"
)
target_include_directories(test_fsdev_is_local_fs PUBLIC
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
)
add_oscap_test("test_fsdev_is_local_fs.sh")

file(GLOB_RECURSE OVAL_RESULTS_SOURCES "${CMAKE_SOURCE_DIR}/src/OVAL/results/oval_cmp*.c")
add_oscap_test_executable(oval_fts_list
	"oval_fts_list.c"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/fsdev.c"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/oval_fts.c"
	"${CMAKE_SOURCE_DIR}/src/common/error.c"
	"${CMAKE_SOURCE_DIR}/src/common/err_queue.c"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/probe/entcmp.c"
	"${CMAKE_SOURCE_DIR}/src/common/util.c"
	"${CMAKE_SOURCE_DIR}/src/common/oscap_pcre.c"
	"${OVAL_RESULTS_SOURCES}"
)
target_include_directories(oval_fts_list PUBLIC
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
	"${CMAKE_SOURCE_DIR}/src/OVAL/results"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/probe"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
	"${CMAKE_SOURCE_DIR}/src/common"
)
target_link_libraries(oval_fts_list openscap)
add_oscap_test("fts.sh")

add_oscap_test_executable(test_memusage
	"test_memusage.c"
	"${CMAKE_SOURCE_DIR}/src/common/bfind.c"
)
target_include_directories(test_memusage PUBLIC
	"${CMAKE_SOURCE_DIR}/src/common"
)
add_oscap_test("test_memusage.sh")
