file(GLOB_RECURSE CPE_SOURCES "*.c")
file(GLOB_RECURSE CPE_HEADERS "*.h")

add_library(cpe_object OBJECT ${CPE_SOURCES} ${CPE_HEADERS})
target_include_directories(cpe_object PUBLIC ${XML2_INCLUDE_DIRS})

add_library(cpe $<TARGET_OBJECTS:cpe_object>)
target_link_libraries(cpe common)

set_target_properties(cpe_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
