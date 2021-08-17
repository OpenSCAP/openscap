include_directories("." "public")

add_definitions(-DPROBE_PLUGIN_EXPORTS)

if (ENABLE_PROBES)
	if(ENABLE_PROBES_WINDOWS)
		add_subdirectory("windows")
	endif()
endif()