add_subdirectory("crapi")

# PThread
find_package(Threads REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD)

add_library(probe
    "probe/fini.c"
    "probe/offline_mode.c"
    "probe/preload.c"
    "probe/init.c"
    "probe/main.c"
    "probe/input_handler.c"
    "probe/input_handler.h"
    "probe/worker.c"
    "probe/worker.h"
    "probe/signal_handler.c"
    "probe/signal_handler.h"
    "probe/probe.h"
    "probe/probe.c"
    "probe/entcmp.c"
    "probe/entcmp.h"
    "probe/icache.c"
    "probe/icache.h"
    "probe/option.c"
    "probe/option.h"
)

target_link_libraries(probe seap oval ${CMAKE_THREAD_LIBS_INIT} ${PCRE_LIBRARIES} ${XML2_LIBRARIES} ${XSLT_LIBRARIES} ${EXSLT_LIBRARIES} ${RPM_LIBRARIES})
target_include_directories(probe PUBLIC "." "public")

include_directories("public")

if(ENABLE_PROBES_INDEPENDENT)
    add_executable(probe_family "independent/family.c")
    target_link_libraries(probe_family probe)
    add_executable(probe_textfilecontent "independent/textfilecontent.c")
    target_link_libraries(probe_textfilecontent probe)
    add_executable(probe_textfilecontent54 "independent/textfilecontent54.c")
    target_link_libraries(probe_textfilecontent54 probe)
    add_executable(probe_variable "independent/variable.c")
    target_link_libraries(probe_variable probe)
    add_executable(probe_xmlfilecontent "independent/xmlfilecontent.c")
    target_link_libraries(probe_xmlfilecontent probe)
    add_executable(probe_filehash "independent/filehash.c")
    target_link_libraries(probe_filehash probe crapi)
    add_executable(probe_filehash58 "independent/filehash58.c")
    target_link_libraries(probe_filehash58 probe crapi)
    add_executable(probe_environmentvariable "independent/environmentvariable.c")
    target_link_libraries(probe_environmentvariable probe)
    add_executable(probe_environmentvariable58 "independent/environmentvariable58.c")
    target_link_libraries(probe_environmentvariable58 probe)
    add_executable(probe_sql "independent/sql.c")

    if(OPENDBX_FOUND)
	target_include_directories(probe_sql PRIVATE ${OPENDBX_INCLUDE_DIRS})
	target_link_libraries(probe_sql probe ${OPENDBX_LIBRARIES})
	add_executable(probe_sql57 "independent/sql57.c")
	target_include_directories(probe_sql57 PRIVATE ${OPENDBX_INCLUDE_DIRS})
	target_link_libraries(probe_sql57 probe ${OPENDBX_LIBRARIES})
    endif()

    if(LDAP_FOUND)
	add_executable(probe_ldap57 "independent/ldap57.c")
	target_include_directories(probe_ldap57 PRIVATE ${LDAP_INCLUDE_DIRS})
	target_link_libraries(probe_ldap57 probe ${LDAP_LIBRARIES})
    endif()
endif()

if(ENABLE_PROBES_UNIX)
    add_executable(probe_dnscache "unix/dnscache.c")
    target_link_libraries(probe_dnscache probe)
    add_executable(probe_runlevel "unix/runlevel.c")
    target_link_libraries(probe_runlevel probe)


    add_executable(probe_file "unix/file.c")
    target_link_libraries(probe_file probe acl)

    add_executable(probe_fileextendedattribute "unix/fileextendedattribute.c")
    target_link_libraries(probe_fileextendedattribute probe)
    add_executable(probe_password "unix/password.c")
    target_link_libraries(probe_password probe)
    add_executable(probe_process "unix/process.c" "unix/process58-devname.c" "unix/process58-devname.h")
    target_link_libraries(probe_process probe ${SELINUX_LIBRARIES})

    if(CAP_FOUND)
	add_executable(probe_process58 "unix/process58.c" "unix/process58-capability.h" "unix/process58-devname.c" "unix/process58-devname.h")
	target_include_directories(probe_process58 PRIVATE ${CAP_INCLUDE_DIR})
	target_link_libraries(probe_process58 probe ${CAP_LIBRARIES} ${SELINUX_LIBRARIES})
    endif()

    add_executable(probe_shadow "unix/shadow.c")
    target_link_libraries(probe_shadow probe)
    add_executable(probe_uname "unix/uname.c")
    target_link_libraries(probe_uname probe)
    add_executable(probe_interface "unix/interface.c")
    target_link_libraries(probe_interface probe)
    add_executable(probe_xinetd "unix/xinetd.c")
    target_link_libraries(probe_xinetd probe)
    add_executable(probe_sysctl "unix/sysctl.c")
    target_link_libraries(probe_sysctl probe)
    add_executable(probe_routingtable "unix/routingtable.c")
    target_link_libraries(probe_routingtable probe)
    add_executable(probe_symlink "unix/symlink.c")
    target_link_libraries(probe_symlink probe)

    if(GCONF_FOUND)
	add_executable(probe_gconf "unix/gconf.c")
	target_include_directories(probe_gconf PRIVATE ${GCONF_INCLUDE_DIRS})
	target_link_libraries(probe_gconf probe ${GCONF_LIBRARIES})
    endif()
endif()

if(ENABLE_PROBES_SOLARIS)
    add_executable(probe_isainfo "unix/solaris/isainfo.c")
    target_link_libraries(probe_isainfo probe)
    add_executable(probe_package "unix/solaris/package.c")
    target_link_libraries(probe_package probe)
    add_executable(probe_patch "unix/solaris/patch.c")
    target_link_libraries(probe_patch probe)
    add_executable(probe_smf "unix/solaris/smf.c")
    target_link_libraries(probe_smf probe)
endif()

if(ENABLE_PROBES_LINUX)
    add_executable(probe_partition "unix/linux/partition.c")
    target_link_libraries(probe_partition probe ${BLKID_LIBRARIES})

    add_executable(probe_inetlisteningservers "unix/linux/inetlisteningservers.c")
    target_link_libraries(probe_inetlisteningservers probe)
    add_executable(probe_iflisteners "unix/linux/iflisteners.c" "unix/linux/iflisteners-proto.h")
    target_link_libraries(probe_iflisteners probe)

    if(SELINUX_FOUND)
	add_executable(probe_selinuxboolean "unix/linux/selinuxboolean.c")
	target_include_directories(probe_selinuxboolean PRIVATE ${SELINUX_INCLUDE_DIR})
	target_link_libraries(probe_selinuxboolean probe ${SELINUX_LIBRARIES})
	add_executable(probe_selinuxsecuritycontext "unix/linux/selinuxsecuritycontext.c")
	target_include_directories(probe_selinuxsecuritycontext PRIVATE ${SELINUX_INCLUDE_DIR})
	target_link_libraries(probe_selinuxsecuritycontext probe ${SELINUX_LIBRARIES})
    endif()

    add_executable(probe_rpminfo "unix/linux/rpminfo.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")
    target_link_libraries(probe_rpminfo probe)
    add_executable(probe_rpmverify "unix/linux/rpmverify.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")
    target_link_libraries(probe_rpmverify probe)
    add_executable(probe_rpmverifyfile "unix/linux/rpmverifyfile.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")
    target_link_libraries(probe_rpmverifyfile probe)
    add_executable(probe_rpmverifypackage "unix/linux/rpmverifypackage.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c" "unix/linux/probe-chroot.h" "unix/linux/probe-chroot.c")
    target_link_libraries(probe_rpmverifypackage probe ${POPT_LIBRARIES})

    if (APTPKG_FOUND)
	add_executable(probe_dpkginfo "unix/linux/dpkginfo.c" "unix/linux/dpkginfo-helper.cxx" "unix/linux/dpkginfo-helper.h")
	target_include_directories(probe_dpkginfo PRIVATE ${APTPKG_INCLUDE_DIR})
	target_link_libraries(probe_dpkginfo probe ${APTPKG_LIBRARIES})
    endif()

    if(DBUS_FOUND)
	add_executable(probe_systemdunitproperty "unix/linux/systemdunitproperty.c" "unix/linux/systemdshared.h")
	target_include_directories(probe_systemdunitproperty PRIVATE ${DBUS_INCLUDE_DIRS})
	target_link_libraries(probe_systemdunitproperty probe ${DBUS_LIBRARIES})
	add_executable(probe_systemdunitdependency "unix/linux/systemdunitdependency.c" "unix/linux/systemdshared.h")
	target_include_directories(probe_systemdunitdependency PRIVATE ${DBUS_INCLUDE_DIRS})
	target_link_libraries(probe_systemdunitdependency probe ${DBUS_LIBRARIES})
    endif()
endif()
