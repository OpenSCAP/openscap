# - Try to find SELinux
# Once done, this will define
#
#  SELINUX_FOUND - system has SELinux
#  SELINUX_INCLUDE_DIRS - the SELinux include directories
#  SELINUX_LIBRARIES - link these to use SELinux

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(SELINUX_PKGCONF libselinux)

# Include dir
find_path(SELINUX_INCLUDE_DIR
	NAMES selinux/selinux.h
	PATHS ${SELINUX_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(SELINUX_LIBRARY
	NAMES selinux
	PATHS ${SELINUX_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SELINUX_PROCESS_INCLUDES SELINUX_INCLUDE_DIR)
set(SELINUX_PROCESS_LIBS SELINUX_LIBRARY)
libfind_process(SELINUX)
