file(GLOB_RECURSE COMMON_SOURCES "*.c")
file(GLOB_RECURSE COMMON_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(common_object OBJECT ${COMMON_SOURCES} ${COMMON_HEADERS})
add_library(common $<TARGET_OBJECTS:common_object>)
target_link_libraries(common ${LIBXSLT_EXSLT_LIBRARIES})

set_oscap_generic_properties(common_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
