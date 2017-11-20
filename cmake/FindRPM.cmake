# - Try to find RPM
# Once done, this will define
#
#  RPM_FOUND - system has RPM
#  RPM_INCLUDE_DIRS - the RPM include directories
#  RPM_LIBRARIES - link these to use RPM

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(RPM_PKGCONF rpm)

# Include dir
find_path(RPM_INCLUDE_DIR
	NAMES rpm
	PATHS ${RPM_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(RPM_LIBRARY
	NAMES rpm
	PATHS ${RPM_PKGCONF_LIBRARY_DIRS}
)
find_library(RPMIO_LIBRARY
	NAMES rpmio
	PATHS ${RPM_PKGCONF_LIBRARY_DIRS}
)

set(RPM_VERSION ${RPM_PKGCONF_VERSION})
if(RPM_VERSION)
	string(COMPARE GREATER "4.6" ${RPM_VERSION} RPM46_FOUND)
	string(COMPARE GREATER "4.7" ${RPM_VERSION} RPM47_FOUND)
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(RPM_PROCESS_INCLUDES RPM_INCLUDE_DIR)
set(RPM_PROCESS_LIBS RPM_LIBRARY RPMIO_LIBRARY)
libfind_process(RPM)
