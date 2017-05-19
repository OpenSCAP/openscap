file(GLOB_RECURSE CCE_SOURCES "*.c")
file(GLOB_RECURSE CEE_HEADERS "*.h")

add_library(cce_object OBJECT ${CCE_SOURCES} ${CEE_HEADERS})

set_target_properties(cce_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
