# - Try to find 
# Once done, this will define
#
# GLib_FOUND - system has GLib
# GLib_INCLUDE_DIRS - the GLib include directories
# GLib_LIBRARIES - link these to use GLib

include(LibFindMacros)

# Dependencies
# The current package name (GLib) is included as first paramater in order to foward the REQUIRED or QUIET paramaters
# to the find_package for the dependent library (ie Threads)
#libfind_package(GLib)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(GLib_PKGCONF glib-2.0)

# Include dir
find_path(GLib_INCLUDE_DIR
  NAMES glib.h
  HINTS ${GLib_PKGCONF_INCLUDE_DIRS}
)
# glibconfig.h is located in a different directory, so include it here
find_path(GLibconfig_INCLUDE_DIR
  NAMES glibconfig.h
  HINTS ${GLib_PKGCONF_INCLUDE_DIRS})

# Finally the library itself
find_library(GLib_LIBRARY
  NAMES glib-2.0
  HINTS ${GLib_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(GLib_PROCESS_INCLUDES GLib_INCLUDE_DIR GLibconfig_INCLUDE_DIR)
set(GLib_PROCESS_LIBS GLib_LIBRARY)
libfind_process(GLib)
