file(GLOB_RECURSE COMPAT_HEADERS "*.h")
list(APPEND COMPAT_SOURCES "")

if(NOT HAVE_STRSEP)
	list(APPEND COMPAT_SOURCES "strsep.c")
endif()

if(NOT HAVE_STRPTIME)
	list(APPEND COMPAT_SOURCES "strptime.c")
endif()

if(NOT HAVE_GETOPT_H)
	list(APPEND COMPAT_SOURCES "getopt.c")
endif()

# dev_to_tty is used in the process and process58 probes that are UNIX only
if(NOT HAVE_DEV_TO_TTY AND UNIX)
	list(APPEND COMPAT_SOURCES "dev_to_tty.c")
endif()

if(COMPAT_SOURCES)
	add_library(compat_object OBJECT ${COMPAT_HEADERS} ${COMPAT_SOURCES})
	set_oscap_generic_properties(compat_object)
endif()
