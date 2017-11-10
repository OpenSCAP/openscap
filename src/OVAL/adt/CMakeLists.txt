file(GLOB_RECURSE ADT_SOURCES "*.c")
file(GLOB_RECURSE ADT_HEADERS "*.h")

add_library(ovaladt_object OBJECT ${ADT_SOURCES} ${ADT_HEADERS})
add_library(ovaladt $<TARGET_OBJECTS:ovaladt_object>)
set_oscap_generic_properties(ovaladt_object)
