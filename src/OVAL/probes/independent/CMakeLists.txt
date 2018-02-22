set (INDEPENDENT_PROBES_SOURCES
	"system_info.c"
	"system_info.h"
	"family.c"
	"family.h"
	"textfilecontent54.c"
	"textfilecontent54.h"
)
add_library(independent_probes_object OBJECT ${INDEPENDENT_PROBES_SOURCES})
set_oscap_generic_properties(independent_probes_object)
