file(GLOB_RECURSE CCE_SOURCES "*.c")
file(GLOB_RECURSE CCE_HEADERS "*.h")

add_library(cce_object OBJECT ${CCE_SOURCES} ${CCE_HEADERS})
set_oscap_generic_properties(cce_object)
