if(ENABLE_OSCAP_UTIL)
	file(GLOB UTILS_SOURCES "*.c")
	add_executable(oscap ${UTILS_SOURCES})
	target_link_libraries(oscap openscap)

	# Install the 'oscap' utility
	if(WIN32)
		set(OSCAP_UTIL_DESTINATION ".")
	else()
		set(OSCAP_UTIL_DESTINATION ${CMAKE_INSTALL_BINDIR})
	endif()
	install(TARGETS "oscap"
		DESTINATION ${OSCAP_UTIL_DESTINATION}
	)

	# Install manual page
	if(NOT WIN32)
		install(FILES "oscap.8"
			DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
		)
	endif()

	# Windows: Install all recursively dependent DLLs for oscap.exe
	if(WIN32)
		set(APPS "\${CMAKE_INSTALL_PREFIX}/oscap.exe") # the \ before $ is required by BundleUtilities
		set(VCPKG_DLL_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
		install(CODE "
			include(BundleUtilities)
			fixup_bundle(\"${APPS}\" \"\" \"${VCPKG_DLL_DIR}\")
		")
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
	configure_file("oscap-docker.in" "oscap-docker" @ONLY)
	execute_process(COMMAND
		${OSCAP_DOCKER_PYTHON} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(False, False, prefix='${CMAKE_INSTALL_PREFIX}'))"
		OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_INSTALL_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
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
if(ENABLE_OSCAP_UTIL_VM)
	install(PROGRAMS "oscap-vm"
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	install(FILES "oscap-vm.8"
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
