add_subdirectory("SEAP")
add_subdirectory("crapi")
add_subdirectory("probe")

include_directories("." "public")

if (ENABLE_PROBES)
	if(ENABLE_PROBES_INDEPENDENT)
		add_subdirectory("independent")
	endif()
endif()
