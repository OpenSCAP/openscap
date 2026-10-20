# - Try to find the Cap development libraries
# Once done this will define
#
# CAP_FOUND - system has libcap-devel
# CAP_INCLUDE_DIR - cap include directory
# CAP_LIBRARIES - cap (if found) library

if(CAP_INCLUDE_DIR AND CAP_LIBRARIES)
    # Already in cache, be silent
    set(CAP_FIND_QUIETLY TRUE)
endif()

find_path(CAP_INCLUDE_DIR sys/capability.h)
find_library(CAP_LIBRARIES NAMES cap)

if(CAP_INCLUDE_DIR AND CAP_LIBRARIES)
   set(CAP_FOUND TRUE)
endif()

if(CAP_FOUND)
   if(NOT CAP_FIND_QUIETLY)
      message(STATUS "Found Cap: ${CAP_LIBRARIES}")
   endif()
else()
   if(Cap_FIND_REQUIRED)
       message(FATAL_ERROR "Could NOT find Cap")
   endif()
endif()

mark_as_advanced(CAP_INCLUDE_DIR CAP_LIBRARIES)
