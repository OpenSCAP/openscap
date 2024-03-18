# - Find pcre
# Find the native PCRE2 headers and libraries.
#
# PCRE2_INCLUDE_DIRS	- where to find pcre2.h, etc.
# PCRE2_LIBRARIES	- List of libraries when using pcre.
# PCRE2_FOUND	- True if pcre found.

# Look for the header file.
FIND_PATH(PCRE2_INCLUDE_DIR pcre2.h)

# Look for the library.
FIND_LIBRARY(PCRE2_LIBRARY NAMES libpcre2.a pcre2-8)

# Handle the QUIETLY and REQUIRED arguments and set PCRE2_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE2 DEFAULT_MSG PCRE2_LIBRARY PCRE2_INCLUDE_DIR)

# Copy the results to the output variables.
IF(PCRE2_FOUND)
    SET(PCRE2_LIBRARIES ${PCRE2_LIBRARY})
    SET(PCRE2_INCLUDE_DIRS ${PCRE2_INCLUDE_DIR})
ELSE(PCRE2_FOUND)
    SET(PCRE_LIBRARIES)
    SET(PCRE_INCLUDE_DIRS)
ENDIF(PCRE2_FOUND)

MARK_AS_ADVANCED(PCRE2_INCLUDE_DIRS PCRE2_LIBRARIES)
