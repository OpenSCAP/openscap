file(GLOB_RECURSE ADT_SOURCES "*.c")
file(GLOB_RECURSE ADT_HEADERS "*.h")

add_library(ovaladt_object OBJECT ${ADT_SOURCES} ${ADT_HEADERS})
add_library(ovaladt $<TARGET_OBJECTS:ovaladt_object>)

set_target_properties(ovaladt_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
