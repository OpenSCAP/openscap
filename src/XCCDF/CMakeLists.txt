file(GLOB_RECURSE XCCDF_SOURCES "*.c")
file(GLOB_RECURSE XCCDF_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(xccdf_object OBJECT ${XCCDF_SOURCES} ${XCCDF_HEADERS})
add_library(xccdf $<TARGET_OBJECTS:xccdf_object>)
target_link_libraries(xccdf common)

set_oscap_generic_properties(xccdf_object)
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
