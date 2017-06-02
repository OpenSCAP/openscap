file(GLOB_RECURSE COMMON_SOURCES "*.c")
file(GLOB_RECURSE COMMON_HEADERS "*.h")

add_library(common_object OBJECT ${COMMON_SOURCES} ${COMMON_HEADERS})
add_library(common $<TARGET_OBJECTS:common_object>)
target_link_libraries(common ${LIBXSLT_EXSLT_LIBRARIES})

set_target_properties(common_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
