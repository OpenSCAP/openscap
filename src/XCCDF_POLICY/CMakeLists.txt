file(GLOB_RECURSE XCCDF_POLICY_SOURCES "*.c")
file(GLOB_RECURSE XCCDF_POLICY_HEADERS "*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "public/*.h")

add_library(xccdfPolicy_object OBJECT ${XCCDF_POLICY_SOURCES} ${XCCDF_POLICY_HEADERS})
set_oscap_generic_properties(xccdfPolicy_object)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openscap)
