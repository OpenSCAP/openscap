if(ENABLE_SCE)
	file(GLOB_RECURSE SCE_SOURCES "*.c")

	add_library(openscap_sce SHARED ${SCE_SOURCES})
	target_include_directories(openscap_sce PUBLIC public)
	set_target_properties(openscap_sce PROPERTIES VERSION ${SONAME} SOVERSION ${SOVERSION})
	target_link_libraries(openscap_sce PRIVATE openscap)

	install(TARGETS openscap_sce
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()
