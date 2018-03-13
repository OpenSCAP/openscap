add_subdirectory("generic/rbt")

file(GLOB_RECURSE SEAP_SOURCES "*.c")
file(GLOB_RECURSE SEAP_HEADERS "*.h")

file(GLOB_RECURSE RBT_SOURCES "generic/rbt/*.c")
file(GLOB_RECURSE RBT_HEADERS "generic/rbt/*.h")

file(GLOB_RECURSE SEAP_SOURCES_TO_REMOVE "sexp-handler.c")
list(REMOVE_ITEM SEAP_SOURCES ${SEAP_SOURCES_TO_REMOVE} ${RBT_SOURCES} ${RBT_HEADERS})

if(ENABLE_PROBES)
	add_library(seap_object OBJECT ${SEAP_SOURCES} ${SEAP_HEADERS})
	target_include_directories(seap_object PRIVATE
		${CMAKE_SOURCE_DIR}
		"public"
	)
	set_oscap_generic_properties(seap_object)
endif()
