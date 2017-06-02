file(GLOB_RECURSE SEAP_SOURCES "*.c")
file(GLOB_RECURSE SEAP_HEADERS "*.h")

file(GLOB_RECURSE RBT_SOURCES "generic/rbt/*.c")
file(GLOB_RECURSE RBT_HEADERS "generic/rbt/*.h")

file(GLOB_RECURSE SEAP_SOURCES_TO_REMOVE "sexp-handler.c" "sexp-template.c")
list(REMOVE_ITEM SEAP_SOURCES ${SEAP_SOURCES_TO_REMOVE} ${RBT_SOURCES} ${RBT_HEADERS})

add_definitions(-D__need_IOV_MAX)
add_library(seap_object OBJECT ${SEAP_SOURCES} ${SEAP_HEADERS})
add_library(seap $<TARGET_OBJECTS:seap_object>)
target_include_directories(seap_object PRIVATE
    "../../../../"
    "public"
)
target_link_libraries(seap rbt)

set_target_properties(seap_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)

add_subdirectory("generic/rbt")
