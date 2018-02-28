set (INDEPENDENT_PROBES_SOURCES
	"system_info_probe.c"
	"system_info_probe.h"
	"family_probe.c"
	"family_probe.h"
	"textfilecontent_probe.c"
	"textfilecontent_probe.h"
	"textfilecontent54_probe.c"
	"textfilecontent54_probe.h"
	"environmentvariable_probe.c"
	"environmentvariable_probe.h"
	"environmentvariable58_probe.c"
	"environmentvariable58_probe.h"
	"variable_probe.c"
	"variable_probe.h"
	"xmlfilecontent_probe.c"
	"xmlfilecontent_probe.h"
)
if (GCRYPT_FOUND OR NSS_FOUND)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"filehash_probe.c"
		"filehash_probe.h"
		"filehash58_probe.c"
		"filehash58_probe.h"
	)
endif()
if (OPENDBX_FOUND)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"sql_probe.c"
		"sql_probe.h"
		"sql57_probe.c"
		"sql57_probe.h"
	)
endif()

add_library(independent_probes_object OBJECT ${INDEPENDENT_PROBES_SOURCES})
set_oscap_generic_properties(independent_probes_object)
