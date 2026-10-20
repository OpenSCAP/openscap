# - Try to find GConf
# Once done, this will define
#
# GCONF_FOUND - system has GCONF
# GCONF_INCLUDE_DIRS - the GCONF include directories
# GCONF_LIBRARIES - link these to use GCONF

include(LibFindMacros)

# Dependencies
# The current package name (GCONF) is included as first paramater in order to foward the REQUIRED or QUIET paramaters
# to the find_package for the dependent library (ie Threads)
libfind_package(GCONF GLib)
libfind_package(GCONF GObject)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(GCONF_PKGCONF gconf-2.0)

# Include dir
find_path(GCONF_INCLUDE_DIR
  NAMES gconf/gconf.h
  HINTS ${GCONF_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(GCONF_LIBRARY
  NAMES gconf-2
  HINTS ${GCONF_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(GCONF_PROCESS_INCLUDES GCONF_INCLUDE_DIR GLib_INCLUDE_DIRS GObject_INCLUDE_DIRS)
set(GCONF_PROCESS_LIBS GCONF_LIBRARY GLib_LIBRARIES GObject_LIBRARIES)
libfind_process(GCONF)

mark_as_advanced(GLib_DIR GObject_DIR)
