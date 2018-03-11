if(OPENSCAP_PROBE_UNIX_DNSCACHE)
	list(APPEND UNIX_PROBES_SOURCES
		"dnscache_probe.c"
		"dnscache_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_FILE)
	list(APPEND UNIX_PROBES_SOURCES
		"file_probe.c"
		"file_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_FILEEXTENDEDATTRIBUTE)
	list(APPEND UNIX_PROBES_SOURCES
		"fileextendedattribute_probe.c"
		"fileextendedattribute_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_GCONF)
	list(APPEND UNIX_PROBES_SOURCES
		"gconf_probe.c"
		"gconf_probe.h"
	)
	list(APPEND UNIX_PROBES_INCLUDE_DIRECTORIES
		${GCONF_INCLUDE_DIRS}
	)
endif()

if(OPENSCAP_PROBE_UNIX_INTERFACE)
	list(APPEND UNIX_PROBES_SOURCES
		"interface_probe.c"
		"interface_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_PASSWORD)
	list(APPEND UNIX_PROBES_SOURCES
		"password_probe.c"
		"password_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_PROCESS)
	list(APPEND UNIX_PROBES_SOURCES
		"process_probe.c"
		"process_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_PROCESS58)
	list(APPEND UNIX_PROBES_SOURCES
		"process58_probe.c"
		"process58_probe.h"
		"process58-capability.h"
		"process58-devname.c"
		"process58-devname.h"
	)
	list(APPEND UNIX_PROBES_INCLUDE_DIRECTORIES
		${CAP_INCLUDE_DIR}
	)
endif()

if(OPENSCAP_PROBE_UNIX_ROUTINGTABLE)
	list(APPEND UNIX_PROBES_SOURCES
		"routingtable_probe.c"
		"routingtable_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_RUNLEVEL)
	list(APPEND UNIX_PROBES_SOURCES
		"runlevel_probe.c"
		"runlevel_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_SHADOW)
	list(APPEND UNIX_PROBES_SOURCES
		"shadow_probe.c"
		"shadow_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_SYMLINK)
	list(APPEND UNIX_PROBES_SOURCES
		"symlink_probe.c"
		"symlink_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_SYSCTL)
	list(APPEND UNIX_PROBES_SOURCES
		"sysctl_probe.c"
		"sysctl_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_UNAME)
	list(APPEND UNIX_PROBES_SOURCES
		"uname_probe.c"
		"uname_probe.h"
	)
endif()

if(OPENSCAP_PROBE_UNIX_XINETD)
	list(APPEND UNIX_PROBES_SOURCES
		"xinetd_probe.c"
		"xinetd_probe.h"
	)
endif()

add_library(unix_probes_object OBJECT ${UNIX_PROBES_SOURCES})
set_oscap_generic_properties(unix_probes_object)
target_include_directories(unix_probes_object PRIVATE ${UNIX_PROBES_INCLUDE_DIRECTORIES})

if(ENABLE_PROBES_LINUX)
	add_subdirectory("linux")
endif()
