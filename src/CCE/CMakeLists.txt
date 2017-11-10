file(GLOB_RECURSE CCE_SOURCES "*.c")
file(GLOB_RECURSE CEE_HEADERS "*.h")

add_library(cce_object OBJECT ${CCE_SOURCES} ${CEE_HEADERS})
set_oscap_generic_properties(cce_object)
