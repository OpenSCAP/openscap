if(ENABLE_PROBES)
	add_subdirectory("probes")
endif()

file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)