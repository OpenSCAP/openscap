# - Try to find BLKID
# Once done, this will define
#
#  BLKID_FOUND - system has BLKID
#  BLKID_INCLUDE_DIRS - the BLKID include directories
#  BLKID_LIBRARIES - link these to use BLKID

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(BLKID_PKGCONF popt)

# Include dir
find_path(BLKID_INCLUDE_DIR
	NAMES blkid/blkid.h
	PATHS ${BLKID_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(BLKID_LIBRARY
	NAMES blkid
	PATHS ${BLKID_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(BLKID_PROCESS_INCLUDES BLKID_INCLUDE_DIR)
set(BLKID_PROCESS_LIBS BLKID_LIBRARY)
libfind_process(BLKID)
