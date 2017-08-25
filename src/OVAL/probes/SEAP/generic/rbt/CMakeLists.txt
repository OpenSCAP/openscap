file(GLOB_RECURSE RBT_SOURCES "*.c")
file(GLOB_RECURSE RBT_HEADERS "*.h")

add_library(rbt_object OBJECT ${RBT_SOURCES} ${RBT_HEADERS})
add_library(rbt $<TARGET_OBJECTS:rbt_object>)

set_target_properties(rbt_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
