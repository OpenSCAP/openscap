# - Try to find 
# Once done, this will define
#
# GObject_FOUND - system has GObject
# GObject_INCLUDE_DIRS - the GObject include directories
# GObject_LIBRARIES - link these to use GObject

include(LibFindMacros)

# Dependencies
# The current package name (GObject) is included as first paramater in order to foward the REQUIRED or QUIET paramaters
# to the find_package for the dependent library (ie Threads)
#libfind_package(GObject)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(GObject_PKGCONF gobject-2.0)

# Include dir
find_path(GObject_INCLUDE_DIR
  NAMES gobject/gobject.h
  HINTS ${GObject_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(GObject_LIBRARY
  NAMES gobject-2.0
  HINTS ${GObject_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(GObject_PROCESS_INCLUDES GObject_INCLUDE_DIR )
set(GObject_PROCESS_LIBS GObject_LIBRARY)
libfind_process(GObject)
