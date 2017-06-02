file(GLOB_RECURSE SOURCE_SOURCES "*.c")
file(GLOB_RECURSE SOURCE_HEADERS "*.h")

add_library(oscapsource_object OBJECT ${SOURCE_SOURCES} ${SOURCE_HEADERS})
add_library(oscapsource $<TARGET_OBJECTS:oscapsource_object>)
target_link_libraries(oscapsource ovalcmp ovaladt xccdf cpe ${LIBXSLT_LIBRARIES})

set_target_properties(oscapsource_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
