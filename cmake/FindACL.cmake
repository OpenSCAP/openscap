# - Try to find ACL
# Once done, this will define
#
#  ACL_FOUND - system has ACL
#  ACL_INCLUDE_DIRS - the ACL include directories
#  ACL_LIBRARIES - link these to use ACL

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(ACL_PKGCONF acl)

# Include dir
find_path(ACL_INCLUDE_DIR
	NAMES acl/libacl.h
	PATHS ${ACL_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(ACL_LIBRARY
	NAMES acl
	PATHS ${ACL_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(ACL_PROCESS_INCLUDES ACL_INCLUDE_DIR)
set(ACL_PROCESS_LIBS ACL_LIBRARY)
libfind_process(ACL)
