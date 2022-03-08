# - Try to find libmagic
# Once done, this will define
#
#  LIBMAGIC_FOUND - system has libmagic
#  LIBMAGIC_INCLUDE_DIRS - the libmagic include directories
#  LIBMAGIC_LIBRARIES - link these to use libmagic

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(LIBMAGIC_PKGCONF magic-0.1)

# Include dir
find_path(LIBMAGIC_INCLUDE_DIR
	NAMES magic.h
	PATHS ${LIBMAGIC_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(LIBMAGIC_LIBRARY
	NAMES libmagic.so
	PATHS ${LIBMAGIC_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(LIBMAGIC_PROCESS_INCLUDES LIBMAGIC_INCLUDE_DIR)
set(LIBMAGIC_PROCESS_LIBS LIBMAGIC_LIBRARY)
libfind_process(LIBMAGIC)
