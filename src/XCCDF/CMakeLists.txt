file(GLOB_RECURSE XCCDF_SOURCES "*.c")
file(GLOB_RECURSE XCCDF_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(xccdf_object OBJECT ${XCCDF_SOURCES} ${XCCDF_HEADERS})
set_oscap_generic_properties(xccdf_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
