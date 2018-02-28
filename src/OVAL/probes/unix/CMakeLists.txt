set (UNIX_PROBES_SOURCES
	"dnscache_probe.c"
	"dnscache_probe.h"
	"file_probe.c"
	"file_probe.h"
	"fileextendedattribute_probe.c"
	"fileextendedattribute_probe.h"
	"interface_probe.c"
	"interface_probe.h"
	"password_probe.c"
	"password_probe.h"
	"process_probe.c"
	"process_probe.h"
	"routingtable_probe.c"
	"routingtable_probe.h"
	"runlevel_probe.c"
	"runlevel_probe.h"
	"shadow_probe.c"
	"shadow_probe.h"
	"symlink_probe.c"
	"symlink_probe.h"
	"sysctl_probe.c"
	"sysctl_probe.h"
	"uname_probe.c"
	"uname_probe.h"
	"xinetd_probe.c"
	"xinetd_probe.h"
)
if(GCONF_FOUND)
	list(APPEND UNIX_PROBES_SOURCES
		"gconf_probe.c"
		"gconf_probe.h"
	)
	list(APPEND UNIX_PROBES_INCLUDE_DIRECTORIES
		${GCONF_INCLUDE_DIRS}
	)
endif()
if(CAP_FOUND)
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

add_library(unix_probes_object OBJECT ${UNIX_PROBES_SOURCES})
set_oscap_generic_properties(unix_probes_object)
target_include_directories(unix_probes_object PRIVATE ${UNIX_PROBES_INCLUDE_DIRECTORIES})

if(ENABLE_PROBES_LINUX)
	add_subdirectory("linux")
endif()