file(GLOB_RECURSE CVSS_SOURCES "*.c")
file(GLOB_RECURSE CVSS_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(cvss_object OBJECT ${CVSS_SOURCES} ${CVSS_HEADERS})
set_oscap_generic_properties(cvss_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
