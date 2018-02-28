set(LINUX_PROBES_SOURCES
	"inetlisteningservers_probe.c"
	"inetlisteningservers_probe.h"
	"iflisteners_probe.c"
	"iflisteners_probe.h"
	"iflisteners-proto.h"
)

if(BLKID_FOUND)
	list(APPEND LINUX_PROBES_SOURCES
		"partition_probe.c"
		"partition_probe.h"
	)
endif()

if(SELINUX_FOUND)
	list(APPEND LINUX_PROBES_SOURCES
		"selinuxboolean_probe.c"
		"selinuxboolean_probe.h"
		"selinuxsecuritycontext_probe.c"
		"selinuxsecuritycontext_probe.h"
	)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${SELINUX_INCLUDE_DIR}
	)
endif()

if(RPM_FOUND)
	list(APPEND LINUX_PROBES_SOURCES
		"probe-chroot.c"
		"probe-chroot.h"
		"rpm-helper.c"
		"rpm-helper.h"
		"rpminfo_probe.c"
		"rpminfo_probe.h"
		"rpmverify_probe.c"
		"rpmverify_probe.h"
		"rpmverifyfile_probe.c"
		"rpmverifyfile_probe.h"
		"rpmverifypackage_probe.c"
		"rpmverifypackage_probe.h"
	)
endif()

if(APTPKG_FOUND)
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

if(DBUS_FOUND)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdshared.h"
		"systemdunitdependency_probe.c"
		"systemdunitdependency_probe.h"
		"systemdunitproperty_probe.c"
		"systemdunitproperty_probe.h"
	)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${DBUS_INCLUDE_DIRS}
	)
endif()

add_library(linux_probes_object OBJECT ${LINUX_PROBES_SOURCES})
set_oscap_generic_properties(linux_probes_object)
target_include_directories(linux_probes_object PRIVATE ${LINUX_PROBES_INCLUDE_DIRECTORIES})