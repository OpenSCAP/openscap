file(GLOB_RECURSE COMPAT_SOURCES "*.h")

if(NOT HAVE_FLOCK)
	list(APPEND COMPAT_SOURCES "flock.c")
endif()

if(NOT HAVE_STRSEP)
	list(APPEND COMPAT_SOURCES "strsep.c")
endif()

if(NOT HAVE_STRPTIME)
	list(APPEND COMPAT_SOURCES "strptime.c")
endif()

add_library(compat_object OBJECT ${COMPAT_SOURCES})
