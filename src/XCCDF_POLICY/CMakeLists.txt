file(GLOB_RECURSE XCCDF_POLICY_SOURCES "*.c")
file(GLOB_RECURSE XCCDF_POLICY_HEADERS "*.h")

add_library(xccdfPolicy_object OBJECT ${XCCDF_POLICY_SOURCES} ${XCCDF_POLICY_HEADERS})
target_include_directories(xccdfPolicy_object PUBLIC ${XML2_INCLUDE_DIRS})

set_target_properties(xccdfPolicy_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
