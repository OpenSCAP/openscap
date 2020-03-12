# - Try to find libyaml
# Once done, this will define
#
#  LIBYAML_FOUND - system has libyaml
#  LIBYAML_INCLUDE_DIRS - the libyaml include directories
#  LIBYAML_LIBRARIES - link these to use libyaml

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(LIBYAML_PKGCONF yaml-0.1)

# Include dir
find_path(LIBYAML_INCLUDE_DIR
	NAMES yaml.h
	PATHS ${LIBYAML_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(LIBYAML_LIBRARY
	NAMES libyaml.so
	PATHS ${LIBYAML_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(LIBYAML_PROCESS_INCLUDES LIBYAML_INCLUDE_DIR)
set(LIBYAML_PROCESS_LIBS LIBYAML_LIBRARY)
libfind_process(LIBYAML)
