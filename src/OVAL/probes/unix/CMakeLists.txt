set (UNIX_PROBES_SOURCES
	"dnscache.c"
	"dnscache.h"
	"file.c"
	"file.h"
	"fileextendedattribute.c"
	"fileextendedattribute.h"
	"interface.c"
	"interface.h"
	"password.c"
	"password.h"
	"process.c"
	"process.h"
	"routingtable.c"
	"routingtable.h"
	"runlevel.c"
	"runlevel.h"
	"shadow.c"
	"shadow.h"
	"symlink.c"
	"symlink.h"
	"sysctl.c"
	"sysctl.h"
	"uname.c"
	"uname.h"
	"xinetd.c"
	"xinetd.h"
)
if(GCONF_FOUND)
	list(APPEND UNIX_PROBES_SOURCES
		"gconf.c"
		"gconf.h"
	)
	list(APPEND UNIX_PROBES_INCLUDE_DIRECTORIES
		${GCONF_INCLUDE_DIRS}
	)
endif()
if(CAP_FOUND)
	list(APPEND UNIX_PROBES_SOURCES
		"process58.c"
		"process58.h"
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
