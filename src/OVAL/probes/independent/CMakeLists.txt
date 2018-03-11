if(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"environmentvariable_probe.c"
		"environmentvariable_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE58)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"environmentvariable58_probe.c"
		"environmentvariable58_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_FAMILY)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"family_probe.c"
		"family_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_FILEHASH)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"filehash_probe.c"
		"filehash_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_FILEHASH58)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"filehash58_probe.c"
		"filehash58_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_SQL)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"sql_probe.c"
		"sql_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_SQL57)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"sql57_probe.c"
		"sql57_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_SYSTEM_INFO)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"system_info_probe.c"
		"system_info_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"textfilecontent_probe.c"
		"textfilecontent_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT54)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"textfilecontent54_probe.c"
		"textfilecontent54_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_VARIABLE)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"variable_probe.c"
		"variable_probe.h"
	)
endif()

if(OPENSCAP_PROBE_INDEPENDENT_XMLFILECONTENT)
	list(APPEND INDEPENDENT_PROBES_SOURCES
		"xmlfilecontent_probe.c"
		"xmlfilecontent_probe.h"
	)
endif()

add_library(independent_probes_object OBJECT ${INDEPENDENT_PROBES_SOURCES})
set_oscap_generic_properties(independent_probes_object)
