add_subdirectory("crapi")
add_subdirectory("probe")

function(add_oscap_probe PROBE_NAME PROBE_SOURCE_FILE)
	add_executable(${PROBE_NAME} ${PROBE_SOURCE_FILE} ${ARGN})
	target_include_directories(${PROBE_NAME} PRIVATE "." "public")
	target_link_libraries(${PROBE_NAME} openscap ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXSLT_EXSLT_LIBRARIES} ${PCRE_LIBRARIES} ${CURL_LIBRARIES} ${RPM_LIBRARIES} ${BZIP2_LIBRARIES})
	install(TARGETS ${PROBE_NAME} DESTINATION ${OVAL_PROBE_DIR})
endfunction()

if(ENABLE_PROBES_INDEPENDENT)
	add_oscap_probe(probe_system_info "independent/system_info.c")

	add_oscap_probe(probe_family "independent/family.c")

	add_oscap_probe(probe_textfilecontent "independent/textfilecontent.c")

	add_oscap_probe(probe_textfilecontent54 "independent/textfilecontent54.c")

	add_oscap_probe(probe_variable "independent/variable.c")

	add_oscap_probe(probe_xmlfilecontent "independent/xmlfilecontent.c")

	add_oscap_probe(probe_filehash "independent/filehash.c")
	target_link_libraries(probe_filehash crapi)

	add_oscap_probe(probe_filehash58 "independent/filehash58.c")
	target_link_libraries(probe_filehash58 crapi)

	add_oscap_probe(probe_environmentvariable "independent/environmentvariable.c")

	add_oscap_probe(probe_environmentvariable58 "independent/environmentvariable58.c")

	if(OPENDBX_FOUND)
		add_oscap_probe(probe_sql "independent/sql.c")
		target_include_directories(probe_sql PRIVATE ${OPENDBX_INCLUDE_DIRS})
		target_link_libraries(probe_sql ${OPENDBX_LIBRARIES})

		add_oscap_probe(probe_sql57 "independent/sql57.c")
		target_include_directories(probe_sql57 PRIVATE ${OPENDBX_INCLUDE_DIRS})
		target_link_libraries(probe_sql57 ${OPENDBX_LIBRARIES})
	endif()

	if(LDAP_FOUND)
		add_oscap_probe(probe_ldap57 "independent/ldap57.c")
		target_include_directories(probe_ldap57 PRIVATE ${LDAP_INCLUDE_DIRS})
		target_link_libraries(probe_ldap57 ${LDAP_LIBRARIES})
	endif()
endif()

if(ENABLE_PROBES_UNIX)
	add_oscap_probe(probe_dnscache "unix/dnscache.c")

	add_oscap_probe(probe_runlevel "unix/runlevel.c")

	add_oscap_probe(probe_file "unix/file.c")
	target_link_libraries(probe_file ${ACL_LIBRARIES})

	add_oscap_probe(probe_fileextendedattribute "unix/fileextendedattribute.c")

	add_oscap_probe(probe_password "unix/password.c")

	add_oscap_probe(probe_process "unix/process.c" "unix/process58-devname.c" "unix/process58-devname.h")
	target_link_libraries(probe_process ${SELINUX_LIBRARIES} ${PROCPS_LIBRARIES})

	if(CAP_FOUND)
		add_oscap_probe(probe_process58 "unix/process58.c" "unix/process58-capability.h" "unix/process58-devname.c" "unix/process58-devname.h")
		target_include_directories(probe_process58 PRIVATE ${CAP_INCLUDE_DIR})
		target_link_libraries(probe_process58 ${CAP_LIBRARIES} ${SELINUX_LIBRARIES} ${PROCPS_LIBRARIES})
	endif()

	add_oscap_probe(probe_shadow "unix/shadow.c")

	add_oscap_probe(probe_uname "unix/uname.c")

	add_oscap_probe(probe_interface "unix/interface.c")

	add_oscap_probe(probe_xinetd "unix/xinetd.c")

	add_oscap_probe(probe_sysctl "unix/sysctl.c")

	add_oscap_probe(probe_routingtable "unix/routingtable.c")

	add_oscap_probe(probe_symlink "unix/symlink.c")

	if(GCONF_FOUND)
		add_oscap_probe(probe_gconf "unix/gconf.c")
		target_include_directories(probe_gconf PRIVATE ${GCONF_INCLUDE_DIRS})
		target_link_libraries(probe_gconf ${GCONF_LIBRARIES})
	endif()
endif()

if(ENABLE_PROBES_SOLARIS)
	add_oscap_probe(probe_isainfo "unix/solaris/isainfo.c")

	add_oscap_probe(probe_package "unix/solaris/package.c")

	add_oscap_probe(probe_patch "unix/solaris/patch.c")

	add_oscap_probe(probe_smf "unix/solaris/smf.c")
endif()

if(ENABLE_PROBES_LINUX)
	add_oscap_probe(probe_partition "unix/linux/partition.c")
	target_link_libraries(probe_partition ${BLKID_LIBRARIES})

	add_oscap_probe(probe_inetlisteningservers "unix/linux/inetlisteningservers.c")

	add_oscap_probe(probe_iflisteners "unix/linux/iflisteners.c" "unix/linux/iflisteners-proto.h")

	if(SELINUX_FOUND)
		add_oscap_probe(probe_selinuxboolean "unix/linux/selinuxboolean.c")
		target_include_directories(probe_selinuxboolean PRIVATE ${SELINUX_INCLUDE_DIR})
		target_link_libraries(probe_selinuxboolean ${SELINUX_LIBRARIES})

		add_oscap_probe(probe_selinuxsecuritycontext "unix/linux/selinuxsecuritycontext.c")
		target_include_directories(probe_selinuxsecuritycontext PRIVATE ${SELINUX_INCLUDE_DIR})
		target_link_libraries(probe_selinuxsecuritycontext ${SELINUX_LIBRARIES})
	endif()

	if(RPM_FOUND)
		add_oscap_probe(probe_rpminfo "unix/linux/rpminfo.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")

		add_oscap_probe(probe_rpmverify "unix/linux/rpmverify.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")

		add_oscap_probe(probe_rpmverifyfile "unix/linux/rpmverifyfile.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")

		add_oscap_probe(probe_rpmverifypackage "unix/linux/rpmverifypackage.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c" "unix/linux/probe-chroot.h" "unix/linux/probe-chroot.c")
		target_link_libraries(probe_rpmverifypackage ${POPT_LIBRARIES})
	endif()

	if (APTPKG_FOUND)
		add_oscap_probe(probe_dpkginfo "unix/linux/dpkginfo.c" "unix/linux/dpkginfo-helper.cxx" "unix/linux/dpkginfo-helper.h")
		target_include_directories(probe_dpkginfo PRIVATE ${APTPKG_INCLUDE_DIR})
		target_link_libraries(probe_dpkginfo ${APTPKG_LIBRARIES})
	endif()

	if(DBUS_FOUND)
		add_oscap_probe(probe_systemdunitproperty "unix/linux/systemdunitproperty.c" "unix/linux/systemdshared.h")
		target_include_directories(probe_systemdunitproperty PRIVATE ${DBUS_INCLUDE_DIRS})
		target_link_libraries(probe_systemdunitproperty ${DBUS_LIBRARIES})

		add_oscap_probe(probe_systemdunitdependency "unix/linux/systemdunitdependency.c" "unix/linux/systemdshared.h")
		target_include_directories(probe_systemdunitdependency PRIVATE ${DBUS_INCLUDE_DIRS})
		target_link_libraries(probe_systemdunitdependency ${DBUS_LIBRARIES})
	endif()
endif()
