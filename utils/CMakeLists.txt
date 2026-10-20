install(PROGRAMS "oscap-run-sce-script"
	DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(ENABLE_OSCAP_UTIL)
	file(GLOB UTILS_SOURCES "*.c")
	if(HAVE_GETOPT_H)
		list(REMOVE_ITEM UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/getopt.c")
	endif()
	add_executable(oscap ${UTILS_SOURCES})
	target_link_libraries(oscap openscap)
	if(WIN32)
		if(NOT MINGW)
			find_path(GETOPT_INCLUDE_DIR getopt.h)
			include(SelectLibraryConfigurations)
			find_library(GETOPT_LIBRARY_RELEASE NAMES getopt)
			find_library(GETOPT_LIBRARY_DEBUG NAMES getopt PATH_SUFFIXES debug/lib)
			select_library_configurations(GETOPT)

			message("GETOPT_INCLUDE_DIR: ${GETOPT_INCLUDE_DIR}")
			message("GETOPT_LIBRARY: ${GETOPT_LIBRARY}")

			target_include_directories(oscap PRIVATE ${GETOPT_INCLUDE_DIR})
			target_link_libraries(oscap ${GETOPT_LIBRARY})
		endif()
		
		set(OSCAP_UTIL_DESTINATION ".")
		# Install the 'oscap' utility
		install(TARGETS "oscap"
			DESTINATION ${OSCAP_UTIL_DESTINATION}
		)
		# Install all recursively dependent DLLs for oscap.exe
		set(APPS "\${CMAKE_INSTALL_PREFIX}/oscap.exe") # the \ before $ is required by BundleUtilities
		set(VCPKG_DLL_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
		install(CODE "
			include(BundleUtilities)
			fixup_bundle(\"${APPS}\" \"\" \"${VCPKG_DLL_DIR}\")
		")
	else()
		set(OSCAP_UTIL_DESTINATION ${CMAKE_INSTALL_BINDIR})
		# Install the 'oscap' utility
		install(TARGETS "oscap"
			DESTINATION ${OSCAP_UTIL_DESTINATION}
		)
		# Install manual page
		install(FILES "oscap.8"
			DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
		)

		add_custom_target(oscap-chrootable-nocap
			COMMAND cp oscap oscap-chrootable
			COMMENT "Copying oscap binary to a buddy binary that awaits chroot blessing by setcap"
			DEPENDS oscap
		)
		add_custom_target(oscap-chrootable
			COMMAND setcap cap_sys_chroot+ep oscap-chrootable
			COMMENT "Generating chroot-capable oscap buddy"
			DEPENDS oscap-chrootable-nocap
		)

		if(WITH_SYSTEMD)
			if (ENABLE_OSCAP_REMEDIATE_SERVICE)
				install(PROGRAMS "oscap-remediate"
					DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}
				)
				install(PROGRAMS "oscap-remediate-offline"
					DESTINATION ${CMAKE_INSTALL_BINDIR}
				)
				install(FILES "oscap-remediate-offline.8"
					DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
				)
			endif()
		endif()
	endif()
endif()
if(ENABLE_OSCAP_UTIL_CHROOT)
	install(PROGRAMS "oscap-chroot"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-chroot.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_DOCKER)
	configure_file("oscap-docker.in" "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/oscap-docker" @ONLY)
	file(
		COPY "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/oscap-docker"
		DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
		FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)

	if(NOT PYTHON_SITE_PACKAGES_INSTALL_DIR)
	execute_process(COMMAND
		${OSCAP_DOCKER_PYTHON} -c "import sysconfig; print(sysconfig.get_path('purelib'))"
		OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_INSTALL_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	endif()

	install(DIRECTORY oscap_docker_python
		DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
	)
	install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/oscap-docker
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-docker.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_SSH)
	install(PROGRAMS "oscap-ssh"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-ssh.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_AUTOTAILOR)
	install(PROGRAMS "autotailor"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "autotailor.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_VM)
	install(PROGRAMS "oscap-vm"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-vm.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_PODMAN)
	install(PROGRAMS "oscap-podman"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-podman.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_IM)
	install(PROGRAMS "oscap-im"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-im.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
if(ENABLE_OSCAP_UTIL_AS_RPM)
	install(PROGRAMS "scap-as-rpm"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "scap-as-rpm.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
endif()
