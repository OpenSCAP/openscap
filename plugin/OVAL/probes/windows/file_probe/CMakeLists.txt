list(APPEND WINDOWS_FILE_PROBE_PLUGIN_SOURCES
	"file_probe.c"
)

add_library(windows_file_probe_plugin_object MODULE ${WINDOWS_FILE_PROBE_PLUGIN_SOURCES})
#set_oscap_generic_properties(windows_file_probe_plugin_object)
target_include_directories(windows_file_probe_plugin_object PRIVATE
	 ${WINDOWS_PROBES_INCLUDE_DIRECTORIES}
	"${CMAKE_SOURCE_DIR}/src/common"
	"${CMAKE_SOURCE_DIR}/src/common/public"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
	"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
)
target_link_libraries(windows_file_probe_plugin_object PRIVATE openscap)
add_custom_command(TARGET windows_file_probe_plugin_object POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"$<TARGET_FILE_DIR:windows_file_probe_plugin_object>/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE_DIR:windows_file_probe_plugin_object>/windows_file_probe_plugin_object.dll"
        "$<TARGET_FILE_DIR:windows_file_probe_plugin_object>/plugins"
)