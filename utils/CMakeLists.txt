file(GLOB UTILS_SOURCES "*.c")

# see https://cmake.org/Wiki/CMake_RPATH_handling

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR})

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


add_executable(oscap ${UTILS_SOURCES})
target_link_libraries(oscap openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES})

install(TARGETS "oscap"
	DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(ENABLE_OSCAP_UTIL)
	install(FILES "oscap.8"
		DESTINATION "${CMAKE_INSTALL_MANDIR}/man8"
	)
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
