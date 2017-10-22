# - Try to find the APTPKG development libraries
# Once done this will define
#
# APTPKG_FOUND - system has libapt-pkg
# APTPKG_INCLUDE_DIR - APTPKG include directory
# APTPKG_LIBRARIES - APTPKG (if found) library

if(APTPKG_INCLUDE_DIR AND APTPKG_LIBRARIES)
    # Already in cache, be silent
    set(APTPKG_FIND_QUIETLY TRUE)
endif()

find_path(APTPKG_INCLUDE_DIR apt-pkg/init.h)
find_library(APTPKG_LIBRARIES NAMES apt-pkg)

if(APTPKG_INCLUDE_DIR AND APTPKG_LIBRARIES)
   set(APTPKG_FOUND TRUE)
endif()

if(APTPKG_FOUND)
   if(NOT APTPKG_FIND_QUIETLY)
      message(STATUS "Found apt-pkg: ${APTPKG_LIBRARIES}")
   endif()
else()
   if(AptPkg_FIND_REQUIRED)
       message(FATAL_ERROR "Could NOT find AptPkg")
   endif()
endif()

mark_as_advanced(APTPKG_INCLUDE_DIR APTPKG_LIBRARIES)
