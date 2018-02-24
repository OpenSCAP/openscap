set (INDEPENDENT_PROBES_SOURCES
	"system_info.c"
	"system_info.h"
	"family.c"
	"family.h"
	"textfilecontent.c"
	"textfilecontent.h"
	"textfilecontent54.c"
	"textfilecontent54.h"
	"environmentvariable.c"
	"environmentvariable.h"
	"environmentvariable58.c"
	"environmentvariable58.h"
	"variable.c"
	"variable.h"
	"xmlfilecontent.c"
	"xmlfilecontent.h"
)
if (GCRYPT_FOUND OR NSS_FOUND)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"filehash.c"
		"filehash.h"
		"filehash58.c"
		"filehash58.h"
	)
endif()
if (OPENDBX_FOUND)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"sql.c"
		"sql.h"
		"sql57.c"
		"sql57.h"
	)
endif()

add_library(independent_probes_object OBJECT ${INDEPENDENT_PROBES_SOURCES})
set_oscap_generic_properties(independent_probes_object)
