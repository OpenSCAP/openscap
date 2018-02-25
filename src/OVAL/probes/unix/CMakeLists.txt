set (UNIX_PROBES_SOURCES
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

add_library(unix_probes_object OBJECT ${UNIX_PROBES_SOURCES})
set_oscap_generic_properties(unix_probes_object)
target_include_directories(unix_probes_object PRIVATE ${UNIX_PROBES_INCLUDE_DIRECTORIES})
