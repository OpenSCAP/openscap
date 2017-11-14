file(GLOB_RECURSE DS_SOURCES "*.c")
file(GLOB_RECURSE DS_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(ds_object OBJECT ${DS_SOURCES} ${DS_HEADERS})
set_oscap_generic_properties(ds_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
