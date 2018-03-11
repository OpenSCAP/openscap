if(OPENSCAP_PROBE_LINUX_DPKGINFO)
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

if(OPENSCAP_PROBE_LINUX_IFLISTENERS)
	list(APPEND LINUX_PROBES_SOURCES
		"iflisteners_probe.c"
		"iflisteners_probe.h"
		"iflisteners-proto.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_INETLISTENINGSERVERS)
	list(APPEND LINUX_PROBES_SOURCES
		"inetlisteningservers_probe.c"
		"inetlisteningservers_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_PARTITION)
	list(APPEND LINUX_PROBES_SOURCES
		"partition_probe.c"
		"partition_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_RPMINFO OR OPENSCAP_PROBE_LINUX_RPMVERIFY OR OPENSCAP_PROBE_LINUX_RPMVERIFYFILE OR OPENSCAP_PROBE_LINUX_RPMVERIFYPACKAGE)
	list(APPEND LINUX_PROBES_SOURCES
		"probe-chroot.c"
		"probe-chroot.h"
		"rpm-helper.c"
		"rpm-helper.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_RPMINFO)
	list(APPEND LINUX_PROBES_SOURCES
		"rpminfo_probe.c"
		"rpminfo_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_RPMVERIFY)
	list(APPEND LINUX_PROBES_SOURCES
		"rpmverify_probe.c"
		"rpmverify_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_RPMVERIFYFILE)
	list(APPEND LINUX_PROBES_SOURCES
		"rpmverifyfile_probe.c"
		"rpmverifyfile_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_RPMVERIFYPACKAGE)
	list(APPEND LINUX_PROBES_SOURCES
		"rpmverifypackage_probe.c"
		"rpmverifypackage_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_SELINUXBOOLEAN OR OPENSCAP_PROBE_LINUX_SELINUXSECURITYCONTEXT)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${SELINUX_INCLUDE_DIR}
	)
endif()

if(OPENSCAP_PROBE_LINUX_SELINUXBOOLEAN)
	list(APPEND LINUX_PROBES_SOURCES
		"selinuxboolean_probe.c"
		"selinuxboolean_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_SELINUXSECURITYCONTEXT)
	list(APPEND LINUX_PROBES_SOURCES
		"selinuxsecuritycontext_probe.c"
		"selinuxsecuritycontext_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_SYSTEMDUNITDEPENDENCY OR OPENSCAP_PROBE_LINUX_SYSTEMDUNITPROPERTY)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdshared.h"
	)
	list(APPEND LINUX_PROBES_INCLUDE_DIRECTORIES
		${DBUS_INCLUDE_DIRS}
	)
endif()

if(OPENSCAP_PROBE_LINUX_SYSTEMDUNITDEPENDENCY)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdunitdependency_probe.c"
		"systemdunitdependency_probe.h"
	)
endif()

if(OPENSCAP_PROBE_LINUX_SYSTEMDUNITPROPERTY)
	list(APPEND LINUX_PROBES_SOURCES
		"systemdunitproperty_probe.c"
		"systemdunitproperty_probe.h"
	)
endif()

add_library(linux_probes_object OBJECT ${LINUX_PROBES_SOURCES})
set_oscap_generic_properties(linux_probes_object)
target_include_directories(linux_probes_object PRIVATE ${LINUX_PROBES_INCLUDE_DIRECTORIES})
