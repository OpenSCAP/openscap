# - Try to find XMLSEC
# Once done, this will define
#
#  XMLSEC_FOUND - system has XMLSEC
#  XMLSEC_INCLUDE_DIRS - the XMLSEC include directories
#  XMLSEC_LIBRARIES - link these to use XMLSEC
#  XMLSEC_DEFINITIONS - definitions to be added to compiler definitions

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(XMLSEC_PKGCONF xmlsec1)

# Include dir
find_path(XMLSEC_INCLUDE_DIR
  NAMES xmlsec/xmlsec.h
  PATHS ${XMLSEC_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES xmlsec1
)

# Finally the library itself
find_library(XMLSEC_LIBRARY
  NAMES libxmlsec1.so libxmlsec1 libxmlsec xmlsec1 xmlsec
  PATHS ${XMLSEC_PKGCONF_LIBRARY_DIRS}
)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(XMLSEC_OPENSSL_PKGCONF xmlsec1-openssl)

# Finally the library itself
find_library(XMLSEC_OPENSSL_LIBRARY
  NAMES libxmlsec1-openssl.so libxmlsec-openssl xmlsec1-openssl xmlsec-openssl
  PATHS ${XMLSEC_OPENSSL_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XMLSEC_PROCESS_INCLUDES XMLSEC_INCLUDE_DIR)
set(XMLSEC_PROCESS_LIBS XMLSEC_LIBRARY XMLSEC_OPENSSL_LIBRARY)
libfind_process(XMLSEC)

set(XMLSEC_DEFINITIONS ${XMLSEC_OPENSSL_PKGCONF_CFLAGS_OTHER})
