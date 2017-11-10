file(GLOB_RECURSE SOURCE_SOURCES "*.c")
file(GLOB_RECURSE SOURCE_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(oscapsource_object OBJECT ${SOURCE_SOURCES} ${SOURCE_HEADERS})
add_library(oscapsource $<TARGET_OBJECTS:oscapsource_object>)
target_link_libraries(oscapsource ovalcmp ovaladt xccdf cpe ${LIBXSLT_LIBRARIES} ${BZIP2_LIBRARIES})

set_oscap_generic_properties(oscapsource_object)
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
