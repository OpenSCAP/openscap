file(GLOB_RECURSE RBT_SOURCES "*.c")
file(GLOB_RECURSE RBT_HEADERS "*.h")

add_library(rbt_object OBJECT ${RBT_SOURCES} ${RBT_HEADERS})
add_library(rbt $<TARGET_OBJECTS:rbt_object>)

set_oscap_generic_properties(rbt_object)
