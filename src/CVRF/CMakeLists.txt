file(GLOB_RECURSE CVRF_SOURCES "*.c")
file(GLOB_RECURSE CVRF_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(cvrf_object OBJECT ${CVRF_SOURCES} ${CVRF_HEADERS})
target_include_directories(cvrf_object PUBLIC "public")
set_oscap_generic_properties(cvrf_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
