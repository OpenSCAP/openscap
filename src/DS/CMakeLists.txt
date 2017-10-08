file(GLOB_RECURSE DS_SOURCES "*.c")
file(GLOB_RECURSE DS_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(ds_object OBJECT ${DS_SOURCES} ${DS_HEADERS})
set_target_properties(ds_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
