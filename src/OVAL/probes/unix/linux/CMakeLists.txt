
if(LINUX_DPKGINFO_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"dpkginfo-helper.cxx"
		"dpkginfo-helper.h"
		"dpkginfo_probe.c"
		"dpkginfo_probe.h"
	)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${APTPKG_INCLUDE_DIR}
	)
endif()

if(LINUX_IFLISTENERS_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"iflisteners_probe.c"
		"iflisteners_probe.h"
		"iflisteners-proto.h"
	)
endif()

if(LINUX_INETLISTENINGSERVERS_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"inetlisteningservers_probe.c"
		"inetlisteningservers_probe.h"
	)
endif()

if(LINUX_PARTITION_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"partition_probe.c"
		"partition_probe.h"
	)
endif()



if(LINUX_RPMINFO_PROBE OR LINUX_RPMVERIFY_PROBE OR LINUX_RPMVERIFYFILE_PROBE OR LINUX_RPMVERIFYPACKAGE_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"probe-chroot.c"
		"probe-chroot.h"
		"rpm-helper.c"
		"rpm-helper.h"
	)
endif()

if(LINUX_RPMINFO_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"rpminfo_probe.c"
		"rpminfo_probe.h"
	)
endif()

if(LINUX_RPMVERIFY_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"rpmverify_probe.c"
		"rpmverify_probe.h"
	)
endif()

if(LINUX_RPMVERIFYFILE_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"rpmverifyfile_probe.c"
		"rpmverifyfile_probe.h"
	)
endif()

if(LINUX_RPMVERIFYPACKAGE_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"rpmverifypackage_probe.c"
		"rpmverifypackage_probe.h"
	)
endif()

if(LINUX_SELINUXBOOLEAN_PROBE OR LINUX_SELINUXSECURITYCONTEXT_PROBE)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${SELINUX_INCLUDE_DIR}
	)
endif()

if(LINUX_SELINUXBOOLEAN_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"selinuxboolean_probe.c"
		"selinuxboolean_probe.h"
	)
endif()

if(LINUX_SELINUXSECURITYCONTEXT_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"selinuxsecuritycontext_probe.c"
		"selinuxsecuritycontext_probe.h"
	)
endif()

if(LINUX_SYSTEMDUNITDEPENDENCY_PROBE OR LINUX_SYSTEMDUNITPROPERTY_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdshared.h"
	)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${DBUS_INCLUDE_DIRS}
	)
endif()

if(LINUX_SYSTEMDUNITDEPENDENCY_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdunitdependency_probe.c"
		"systemdunitdependency_probe.h"
	)
endif()

if(LINUX_SYSTEMDUNITPROPERTY_PROBE)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdunitproperty_probe.c"
		"systemdunitproperty_probe.h"
	)
endif()

add_library(linux_probes_object OBJECT ${LINUX_PROBES_SOURCES})
set_oscap_generic_properties(linux_probes_object)
target_include_directories(linux_probes_object PRIVATE ${LINUX_PROBES_INCLUDE_DIRECTORIES})