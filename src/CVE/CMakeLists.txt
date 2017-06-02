file(GLOB_RECURSE CVE_SOURCES "*.c")
file(GLOB_RECURSE CVE_HEADERS "*.h")

add_library(cve_object OBJECT ${CVE_SOURCES} ${CVE_HEADERS})

set_target_properties(cve_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
