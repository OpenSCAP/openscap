set (UNIX_PROBES_SOURCES
	"file.c"
	"file.h"
	"fileextendedattribute.c"
	"fileextendedattribute.h"
)

add_library(unix_probes_object OBJECT ${UNIX_PROBES_SOURCES})
set_oscap_generic_properties(unix_probes_object)
