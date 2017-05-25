file(GLOB_RECURSE CVSS_SOURCES "*.c")
file(GLOB_RECURSE CVSS_HEADERS "*.h")

add_library(cvss_object OBJECT ${CVSS_SOURCES} ${CVSS_HEADERS})
target_include_directories(cvss_object PUBLIC ${XML2_INCLUDE_DIRS})

set_target_properties(cvss_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
