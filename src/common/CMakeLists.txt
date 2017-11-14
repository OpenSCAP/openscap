file(GLOB_RECURSE COMMON_SOURCES "*.c")
file(GLOB_RECURSE COMMON_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(common_object OBJECT ${COMMON_SOURCES} ${COMMON_HEADERS})
set_oscap_generic_properties(common_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
