# - Try to found the auditd librairies
# Once done this will define
#
#
# LIBAUDIT_FOUND - system has auditd
# LIBAUDIT_INCLUDE_DIRS - the auditd include directory
# LIBAUDIT_LIBRARIES - the auditd library

FIND_PATH(LIBAUDIT_INCLUDE_DIRS libaudit.h)
FIND_LIBRARY(LIBAUDIT_LIBRARIES NAMES audit)

if(LIBAUDIT_INCLUDE_DIRS AND LIBAUDIT_LIBRARIES)
   set(LIBAUDIT_FOUND TRUE)
endif()

if(LIBAUDIT_FOUND)
    message(STATUS "Found libaudit: ${LIBAUDIT_LIBRARIES}")
else()
   if(Ldap_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find libaudit")
   endif()
endif()

MARK_AS_ADVANCED(LIBAUDIT_INCLUDE_DIRS LIBAUDIT_LIBRARIES)
