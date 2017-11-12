file(GLOB_RECURSE CPE_SOURCES "*.c")
file(GLOB_RECURSE CPE_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(cpe_object OBJECT ${CPE_SOURCES} ${CPE_HEADERS})
set_oscap_generic_properties(cpe_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
