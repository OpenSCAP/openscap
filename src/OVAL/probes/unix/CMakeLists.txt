if(UNIX_DNSCACHE_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"dnscache_probe.c"
		"dnscache_probe.h"
	)
endif()

if(UNIX_FILE_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"file_probe.c"
		"file_probe.h"
	)
endif()

if(UNIX_FILEEXTENDEDATTRIBUTE_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"fileextendedattribute_probe.c"
		"fileextendedattribute_probe.h"
	)
endif()

if(UNIX_GCONF_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"gconf_probe.c"
		"gconf_probe.h"
	)
	list(APPEND UNIX_PROBES_INCLUDE_DIRECTORIES
		${GCONF_INCLUDE_DIRS}
	)
endif()

if(UNIX_INTERFACE_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"interface_probe.c"
		"interface_probe.h"
	)
endif()

if(UNIX_PASSWORD_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"password_probe.c"
		"password_probe.h"
	)
endif()

if(UNIX_PROCESS_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"process_probe.c"
		"process_probe.h"
	)
endif()

if(UNIX_PROCESS58_PROBE)
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

if(UNIX_ROUTINGTABLE_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"routingtable_probe.c"
		"routingtable_probe.h"
	)
endif()

if(UNIX_RUNLEVEL_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"runlevel_probe.c"
		"runlevel_probe.h"
	)
endif()

if(UNIX_SHADOW_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"shadow_probe.c"
		"shadow_probe.h"
	)
endif()

if(UNIX_SYMLINK_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"symlink_probe.c"
		"symlink_probe.h"
	)
endif()

if(UNIX_SYSCTL_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"sysctl_probe.c"
		"sysctl_probe.h"
	)
endif()

if(UNIX_UNAME_PROBE)
	list(APPEND UNIX_PROBES_SOURCES
		"uname_probe.c"
		"uname_probe.h"
	)
endif()

if(UNIX_XINETD_PROBE)
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