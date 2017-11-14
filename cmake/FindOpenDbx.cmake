# - Try to find the OpenDbx development libraries
# Once done this will define
#
#  OPENDBX_FOUND - system has opendbx-devel
#  OPENDBX_INCLUDE_DIR - opendbx include directory
#  OPENDBX_LIBRARIES - opendbx (if found) library

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(OPENDBX_PKGCONF opendbx)

# Include dir
find_path(OPENDBX_INCLUDE_DIR
	NAMES opendbx/api.h
	PATHS ${OPENDBX_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(OPENDBX_LIBRARY
	NAMES opendbx
	PATHS ${OPENDBX_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(OPENDBX_PROCESS_INCLUDES OPENDBX_INCLUDE_DIR)
set(OPENDBX_PROCESS_LIBS OPENDBX_LIBRARY)
libfind_process(OPENDBX)
