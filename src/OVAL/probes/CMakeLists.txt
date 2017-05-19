find_package(DBus REQUIRED)
include_directories(${DBUS_INCLUDE_DIRS})

find_package(Threads REQUIRED)
include_directories(${DBUS_INCLUDE_DIRS})

find_package(Ldap REQUIRED)
include_directories(${LDAP_INCLUDE_DIR})

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
target_link_libraries(probe seap oval ${CMAKE_THREAD_LIBS_INIT} ${PCRE_LIBRARIES} ${LIBXML2_LIBRARIES})

add_definitions(-D_GNU_SOURCE)
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
#add_executable(probe_filehash "independent/filehash.c")
#target_link_libraries(probe_filehash probe crapi)
#add_executable(probe_filehash58 "independent/filehash58.c")
#target_link_libraries(probe_filehash58 probe crapi)
add_executable(probe_environmentvariable "independent/environmentvariable.c")
target_link_libraries(probe_environmentvariable probe)
add_executable(probe_environmentvariable58 "independent/environmentvariable58.c")
target_link_libraries(probe_environmentvariable58 probe)
#add_executable(probe_sql "independent/sql.c")
#target_link_libraries(probe_sql probe opendbx)
#add_executable(probe_sql57 "independent/sql57.c")
#target_link_libraries(probe_sql57 probe opendbx)
add_executable(probe_ldap57 "independent/ldap57.c")
target_link_libraries(probe_ldap57 probe ${LDAP_LIBRARIES})

add_executable(probe_dnscache "unix/dnscache.c")
target_link_libraries(probe_dnscache probe)
add_executable(probe_runlevel "unix/runlevel.c")
target_link_libraries(probe_runlevel probe)
add_executable(probe_file "unix/file.c")
target_link_libraries(probe_file probe)
add_executable(probe_fileextendedattribute "unix/fileextendedattribute.c")
target_link_libraries(probe_fileextendedattribute probe)
add_executable(probe_password "unix/password.c")
target_link_libraries(probe_password probe)
add_executable(probe_process "unix/process.c" "unix/process58-devname.c" "unix/process58-devname.h")
target_link_libraries(probe_process probe)
add_executable(probe_process58 "unix/process58.c" "unix/process58-capability.h" "unix/process58-devname.c" "unix/process58-devname.h")
target_link_libraries(probe_process58 probe cap)
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
#add_executable(probe_gconf "unix/gconf.c")
#target_link_libraries(probe_gconf probe)

#add_executable(probe_isainfo "unix/solaris/isainfo.c")
#target_link_libraries(probe_isainfo probe)
#add_executable(probe_package "unix/solaris/package.c")
#target_link_libraries(probe_package probe)
#add_executable(probe_patch "unix/solaris/patch.c")
#target_link_libraries(probe_patch probe)
#add_executable(probe_smf "unix/solaris/smf.c")
#target_link_libraries(probe_smf probe)

add_executable(probe_partition "unix/linux/partition.c")
target_link_libraries(probe_partition probe)
add_executable(probe_inetlisteningservers "unix/linux/inetlisteningservers.c")
target_link_libraries(probe_inetlisteningservers probe)
add_executable(probe_iflisteners "unix/linux/iflisteners.c" "unix/linux/iflisteners-proto.h")
target_link_libraries(probe_iflisteners probe)
add_executable(probe_selinuxboolean "unix/linux/selinuxboolean.c")
target_link_libraries(probe_selinuxboolean probe selinux)
add_executable(probe_selinuxsecuritycontext "unix/linux/selinuxsecuritycontext.c")
target_link_libraries(probe_selinuxsecuritycontext probe selinux)
#add_executable(probe_rpminfo "unix/linux/rpminfo.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")
#target_link_libraries(probe_rpminfo probe)
#add_executable(probe_rpmverify "unix/linux/rpmverify.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")
#target_link_libraries(probe_rpmverify probe)
#add_executable(probe_rpmverifyfile "unix/linux/rpmverifyfile.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c")
#target_link_libraries(probe_rpmverifyfile probe)
#add_executable(probe_rpmverifypackage "unix/linux/rpmverifypackage.c" "unix/linux/rpm-helper.h" "unix/linux/rpm-helper.c" "unix/linux/probe-chroot.h" "unix/linux/probe-chroot.c")
#target_link_libraries(probe_rpmverifypackage probe)
#add_executable(probe_dpkginfo "unix/linux/dpkginfo.c" "unix/linux/dpkginfo-helper.cxx" "unix/linux/dpkginfo-helper.h")
#target_link_libraries(probe_dpkginfo probe)
add_executable(probe_systemdunitproperty "unix/linux/systemdunitproperty.c" "unix/linux/systemdshared.h")
target_link_libraries(probe_systemdunitproperty probe ${DBUS_LIBRARIES})
add_executable(probe_systemdunitdependency "unix/linux/systemdunitdependency.c" "unix/linux/systemdshared.h")
target_link_libraries(probe_systemdunitdependency probe ${DBUS_LIBRARIES})

add_subdirectory(./SEAP)
#add_subdirectory(./crapi)
