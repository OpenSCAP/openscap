# - Try to find POPT
# Once done, this will define
#
#  POPT_FOUND - system has POPT
#  POPT_INCLUDE_DIRS - the POPT include directories
#  POPT_LIBRARIES - link these to use POPT

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(POPT_PKGCONF popt)

# Include dir
find_path(POPT_INCLUDE_DIR
	NAMES popt.h
	PATHS ${POPT_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(POPT_LIBRARY
	NAMES popt
	PATHS ${POPT_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(POPT_PROCESS_INCLUDES POPT_INCLUDE_DIR)
set(POPT_PROCESS_LIBS POPT_LIBRARY)
libfind_process(POPT)
