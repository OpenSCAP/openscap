# - Try to find yamlpath
# Once done, this will define
#
#  YAMLPATH_FOUND - system has yamlpath
#  YAMLPATH_INCLUDE_DIRS - the yamlpath include directories
#  YAMLPATH_LIBRARIES - link these to use yamlpath

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(YAMLPATH_PKGCONF yaml-path)

# Include dir
find_path(YAMLPATH_INCLUDE_DIR
	NAMES yaml-path.h
	PATHS ${YAMLPATH_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(YAMLPATH_LIBRARY
	NAMES libyaml-path.so
	PATHS ${YAMLPATH_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(YAMLPATH_PROCESS_INCLUDES YAMLPATH_INCLUDE_DIR)
set(YAMLPATH_PROCESS_LIBS YAMLPATH_LIBRARY)
libfind_process(YAMLPATH)
