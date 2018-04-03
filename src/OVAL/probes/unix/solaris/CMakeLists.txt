if(OPENSCAP_PROBE_SOLARIS_ISAINFO)
	list(APPEND SOLARIS_PROBES_SOURCES
		"isainfo_probe.c"
		"isainfo_probe.h"
	)
endif()

add_library(solaris_probes_object OBJECT ${SOLARIS_PROBES_SOURCES})
set_oscap_generic_properties(solaris_probes_object)
target_include_directories(solaris_probes_object PRIVATE ${SOLARIS_PROBES_INCLUDE_DIRECTORIES})