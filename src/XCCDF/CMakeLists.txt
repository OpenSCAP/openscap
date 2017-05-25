file(GLOB_RECURSE XCCDF_SOURCES "*.c")
file(GLOB_RECURSE XCCDF_HEADERS "*.h")

add_library(xccdf_object OBJECT ${XCCDF_SOURCES} ${XCCDF_HEADERS})
target_include_directories(xccdf_object PUBLIC ${XML2_INCLUDE_DIRS})

add_library(xccdf $<TARGET_OBJECTS:xccdf_object>)
target_link_libraries(xccdf common)

set_target_properties(xccdf_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
