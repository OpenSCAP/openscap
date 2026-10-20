# - Try to find PROCPS
# Once done, this will define
#
#  PROCPS_FOUND - system has PROCPS
#  PROCPS_INCLUDE_DIRS - the PROCPS include directories
#  PROCPS_LIBRARIES - link these to use PROCPS

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(PROCPS_PKGCONF libprocps)

# Include dir
find_path(PROCPS_INCLUDE_DIR
	NAMES proc/procps.h
	PATHS ${PROCPS_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(PROCPS_LIBRARY
	NAMES procps
	PATHS ${PROCPS_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(PROCPS_PROCESS_INCLUDES PROCPS_INCLUDE_DIR)
set(PROCPS_PROCESS_LIBS PROCPS_LIBRARY)
libfind_process(PROCPS)
