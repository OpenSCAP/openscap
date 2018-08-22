add_subdirectory("common")
add_subdirectory("CPE")
add_subdirectory("CVE")
add_subdirectory("CVSS")
add_subdirectory("CVRF")
add_subdirectory("DS")
add_subdirectory("OVAL")
if(ENABLE_SCE)
	add_subdirectory("SCE")
endif()
add_subdirectory("source")
add_subdirectory("XCCDF")
add_subdirectory("XCCDF_POLICY")

set(OBJECTS_TO_LINK_AGAINST
	$<TARGET_OBJECTS:common_object>
	$<TARGET_OBJECTS:cpe_object>
	$<TARGET_OBJECTS:cve_object>
	$<TARGET_OBJECTS:cvrf_object>
	$<TARGET_OBJECTS:cvss_object>
	$<TARGET_OBJECTS:ds_object>
	$<TARGET_OBJECTS:oscapsource_object>
	$<TARGET_OBJECTS:oval_object>
	$<TARGET_OBJECTS:ovaladt_object>
	$<TARGET_OBJECTS:ovalcmp_object>
	$<TARGET_OBJECTS:ovalresults_object>
	$<TARGET_OBJECTS:rbt_object>
	$<TARGET_OBJECTS:xccdf_object>
	$<TARGET_OBJECTS:xccdfPolicy_object>
)
if (TARGET "compat_object")
	list(APPEND OBJECTS_TO_LINK_AGAINST $<TARGET_OBJECTS:compat_object>)
endif()
if (ENABLE_PROBES)
	list(APPEND OBJECTS_TO_LINK_AGAINST
		$<TARGET_OBJECTS:probe_object>
		$<TARGET_OBJECTS:seap_object>
	)
	if (ENABLE_PROBES_INDEPENDENT)
		list(APPEND OBJECTS_TO_LINK_AGAINST
			$<TARGET_OBJECTS:independent_probes_object>
		)
	endif()
	if (ENABLE_PROBES_UNIX)
		list(APPEND OBJECTS_TO_LINK_AGAINST
			$<TARGET_OBJECTS:unix_probes_object>
		)
		if (ENABLE_PROBES_LINUX)
			list(APPEND OBJECTS_TO_LINK_AGAINST
				$<TARGET_OBJECTS:linux_probes_object>
			)
		endif()
		if (ENABLE_PROBES_SOLARIS)
			list(APPEND OBJECTS_TO_LINK_AGAINST
				$<TARGET_OBJECTS:solaris_probes_object>
			)
		endif()
	endif()
	if (ENABLE_PROBES_WINDOWS)
		list(APPEND OBJECTS_TO_LINK_AGAINST
			$<TARGET_OBJECTS:windows_probes_object>
		)
	endif()
endif()
if (HAVE_MMAN_H AND (GCRYPT_FOUND OR NSS_FOUND))
	list(APPEND OBJECTS_TO_LINK_AGAINST
		$<TARGET_OBJECTS:crapi_object>
	)
endif()

add_library(openscap SHARED ${OBJECTS_TO_LINK_AGAINST})
add_library(openscap_testing SHARED ${OBJECTS_TO_LINK_AGAINST})

# default symbol visibility is hidden
# exported symbols must be marked by OSCAP_API
set_target_properties(openscap PROPERTIES
	VERSION ${SONAME}
	SOVERSION ${SOVERSION}
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(VERSION_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/libopenscap.map")
	add_custom_command(
		OUTPUT "${VERSION_SCRIPT}"
		COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/generate_export_map.py" --output "${VERSION_SCRIPT}" "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMENT "Generating export map"
	)
	add_custom_target(export_map DEPENDS "${VERSION_SCRIPT}")
	add_dependencies(openscap export_map)
	set_target_properties(openscap PROPERTIES
		LINK_FLAGS "-Wl,--version-script=\"${VERSION_SCRIPT}\""
	)
endif()

set(OPENSCAP_LINKED_LIBRARIES ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
if (BZIP2_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${BZIP2_LIBRARIES})
endif()
if(RPM_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${RPM_LIBRARIES})
endif()
if(WIN32)
	list(APPEND OPENSCAP_LINKED_LIBRARIES wsock32 ws2_32 iphlpapi netapi32 wbemuuid)
endif()
if (GCRYPT_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${GCRYPT_LIBRARIES})
endif()
if (NSS_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${NSS_LIBRARIES})
endif()
if(OPENDBX_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${OPENDBX_LIBRARIES})
endif()
if(GCONF_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${GCONF_LIBRARIES})
endif()
if(SELINUX_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${SELINUX_LIBRARIES})
endif()
if(PROCPS_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${PROCPS_LIBRARIES})
endif()
if(BLKID_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${BLKID_LIBRARIES})
endif()
if(DBUS_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${DBUS_LIBRARIES})
endif()
if(APTPKG_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${APTPKG_LIBRARIES})
endif()
if(ACL_FOUND)
	list(APPEND OPENSCAP_LINKED_LIBRARIES ${ACL_LIBRARY})
endif()
target_link_libraries(openscap ${OPENSCAP_LINKED_LIBRARIES})
target_link_libraries(openscap_testing ${OPENSCAP_LINKED_LIBRARIES})

if(WIN32)
	set(OPENSCAP_INSTALL_DESTINATION ".")
else()
	set(OPENSCAP_INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

install(TARGETS openscap DESTINATION ${OPENSCAP_INSTALL_DESTINATION})
