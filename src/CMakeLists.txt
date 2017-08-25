if (ENABLE_CCE)
    add_subdirectory("CCE")
endif()
add_subdirectory("common")
add_subdirectory("CPE")
add_subdirectory("CVE")
add_subdirectory("CVSS")
add_subdirectory("DS")
add_subdirectory("OVAL")
add_subdirectory("source")
add_subdirectory("XCCDF")
add_subdirectory("XCCDF_POLICY")

set(OBJECTS_TO_LINK_AGAINST
    $<TARGET_OBJECTS:common_object> $<TARGET_OBJECTS:cpe_object> $<TARGET_OBJECTS:cve_object> $<TARGET_OBJECTS:cvss_object>
    $<TARGET_OBJECTS:ds_object> $<TARGET_OBJECTS:oval_object> $<TARGET_OBJECTS:ovaladt_object> $<TARGET_OBJECTS:ovalcmp_object>
    $<TARGET_OBJECTS:ovalresults_object> $<TARGET_OBJECTS:oscapsource_object> $<TARGET_OBJECTS:rbt_object>
    $<TARGET_OBJECTS:seap_object> $<TARGET_OBJECTS:xccdf_object> $<TARGET_OBJECTS:xccdfPolicy_object>
)
if (ENABLE_CCE)
    list(APPEND OBJECTS_TO_LINK_AGAINST $<TARGET_OBJECTS:cce_object>)
endif()

add_library(openscap SHARED ${OBJECTS_TO_LINK_AGAINST})
