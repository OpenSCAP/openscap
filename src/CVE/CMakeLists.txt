file(GLOB_RECURSE CVE_SOURCES "*.c")
file(GLOB_RECURSE CVE_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(cve_object OBJECT ${CVE_SOURCES} ${CVE_HEADERS})
set_oscap_generic_properties(cve_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
