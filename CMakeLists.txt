cmake_minimum_required(VERSION 2.6)

project("openscap")
set(OPENSCAP_VERSION_MAJOR "1")
set(OPENSCAP_VERSION_MINOR "3")
set(OPENSCAP_VERSION_PATCH "0")
set(OPENSCAP_VERSION "${OPENSCAP_VERSION_MAJOR}.${OPENSCAP_VERSION_MINOR}.${OPENSCAP_VERSION_PATCH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(NDEBUG 1)
endif()

# ---------- DEPENDENCIES

include(GNUInstallDirs)
include(FindPkgConfig)
include(CheckLibraryExists)

# ACL
#pkg_check_modules(ACL REQUIRED acl)
CHECK_LIBRARY_EXISTS(acl acl_init "" HAVE_ACL_INIT)
CHECK_LIBRARY_EXISTS(acl acl_extended_file "" HAVE_ACL_EXTENDED_FILE)

# APTPKG
pkg_check_modules(APTPKG libapt-pkg)
if(APTPKG_FOUND)
    CHECK_LIBRARY_EXISTS(${APTPKG_LIBRARIES} pkgVersion ${APTPKG_LIBRARY_DIRS} HAVE_PKGVERSION)
endif()

# BLKID
pkg_check_modules(BLKID REQUIRED blkid)
if(BLKID_FOUND)
    CHECK_LIBRARY_EXISTS(blkid blkid_get_cache "" HAVE_BLKID_GET_CACHE)
    CHECK_LIBRARY_EXISTS(blkid blkid_get_tag_value "" HAVE_BLKID_GET_TAG_VALUE)
endif()

# CAP
pkg_check_modules(CAP REQUIRED libcap)
CHECK_LIBRARY_EXISTS(cap cap_init "" HAVE_CAP_INIT)
CHECK_LIBRARY_EXISTS(cap cap_get_pid "" HAVE_CAP_GET_PID)
CHECK_LIBRARY_EXISTS(cap capgetp "" HAVE_CAPGETP)

# CURL
pkg_check_modules(CURL REQUIRED libcurl>=7.12.0)

# DBUS
pkg_check_modules(DBUS REQUIRED dbus-1)
if(DBUS_FOUND)
    CHECK_LIBRARY_EXISTS(${DBUS_LIBRARIES} dbus_bus_get "" HAVE_DBUS_BUS_GET)
endif()

# EXSLT
pkg_check_modules(EXSLT REQUIRED libexslt>=0.8)

# GCONF2
pkg_check_modules(GCONF2 REQUIRED gconf-2.0)
if(GCONF2_FOUND)
    CHECK_LIBRARY_EXISTS(gconf-2 gconf_engine_get_default "" HAVE_GCONF_ENGINE_GET_DEFAULT)
endif()

# LDAP
find_package(Ldap REQUIRED)
CHECK_LIBRARY_EXISTS(ldap ldap_init "" HAVE_LDAP_INIT)
CHECK_LIBRARY_EXISTS(lber ber_init "" HAVE_BER_INIT)

# OpenDbx
pkg_check_modules(OPENDBX REQUIRED opendbx)
CHECK_LIBRARY_EXISTS(opendbx odbx_init "" HAVE_ODBX_INIT)

# PCRE
pkg_check_modules(PCRE REQUIRED libpcre)
if(PCRE_FOUND)
    CHECK_LIBRARY_EXISTS(pcre pcre_exec "" HAVE_PCRE_EXEC)
endif()

# POPT
pkg_check_modules(POPT REQUIRED popt)

# PROCPS
pkg_check_modules(PROCPS libprocps)
if(PROCPS_FOUND)
    CHECK_LIBRARY_EXISTS(procps dev_to_tty "" HAVE_DEV_TO_TTY)
endif()

# RPM
pkg_check_modules(RPM rpm>=4.4)
pkg_check_modules(RPM46 rpm>=4.6)
pkg_check_modules(RPM47 rpm>=4.7)
if(RPM_FOUND)
    CHECK_LIBRARY_EXISTS(rpm rpmtsCreate "" HAVE_RPMTSCREATE)
    CHECK_LIBRARY_EXISTS(rpm rpmReadConfigFiles "" HAVE_RPMREADCONFIGFILES)
    CHECK_LIBRARY_EXISTS(rpm headerFormat "" HAVE_HEADERFORMAT)
    CHECK_LIBRARY_EXISTS(rpm headerSprintf "" HAVE_HEADERSPRINTF)
    CHECK_LIBRARY_EXISTS(rpmio rpmFreeCrypto "" HAVE_RPMFREECRYPTO)
    CHECK_LIBRARY_EXISTS(rpm rpmFreeFilesystems "" HAVE_RPMFREEFILESYSTEMS)

    set(HAVE_RPMVERCMP 1)
endif()
if (RPM46_FOUND)
    set(HAVE_RPM46 1)
endif()
if (RPM47_FOUND)
    set(HAVE_RPM47 1)
endif()

# SELINUX
pkg_check_modules(SELINUX REQUIRED libselinux)

# XML2
pkg_check_modules(XML2 REQUIRED libxml-2.0>=2.0)
if (XML2_FOUND)
    CHECK_LIBRARY_EXISTS(xml2 xmlTextReaderRead "" HAVE_XMLTEXTREADERREAD)
endif()

# XSTL
pkg_check_modules(XSLT REQUIRED libxslt>=1.1)
if (XSLT_FOUND)
    CHECK_LIBRARY_EXISTS(xslt xsltDocumentFunction "" HAVE_XSLTDOCUMENTFUNCTION)
endif()

# BZip2
find_package(BZip2)

# PThread
find_package(Threads)
set(CMAKE_THREAD_PREFER_PTHREAD)

# WITH_CRYPTO
set(WITH_CRYPTO "gcrypt" CACHE STRING "gcrypt|nss3")
if(NOT (${WITH_CRYPTO} EQUAL "nss3"))
    # gcrypt
    find_package(GCrypt REQUIRED)
    include(CheckCSourceCompiles)
    CHECK_C_SOURCE_COMPILES("#include <gcrypt.h>\nint main() {return GCRYCTL_SET_ENFORCED_FIPS_FLAG;}" HAVE_GCRYCTL_SET_ENFORCED_FIPS_FLAG)
else()
    # nss3
    pkg_check_modules(NSS3 REQUIRED nss>=3.0)
endif()

# HAVE FUNCTIONS
CHECK_LIBRARY_EXISTS(pthread pthread_timedjoin_np "" HAVE_PTHREAD_TIMEDJOIN_NP)
CHECK_LIBRARY_EXISTS(pthread pthread_setname_np "" HAVE_PTHREAD_SETNAME_NP)
CHECK_LIBRARY_EXISTS(pthread pthread_getname_np "" HAVE_PTHREAD_GETNAME_NP)
CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_CLOCK_GETTIME)

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(fts_open have_fts_open)
CHECK_FUNCTION_EXISTS(posix_memalign HAVE_POSIX_MEMALIGN)
CHECK_FUNCTION_EXISTS(memalign HAVE_MEMALIGN)

# HAVE HEADERS
include(CheckIncludeFile)
CHECK_INCLUDE_FILE(syslog.h HAVE_SYSLOG_H)
CHECK_INCLUDE_FILE(ftw.h HAVE_FTW_H)
CHECK_INCLUDE_FILE(stdio_ext.h HAVE_STDIO_EXT_H)
CHECK_INCLUDE_FILE(sys/capability.h HAVE_SYS_CAPABILITY_H)
CHECK_INCLUDE_FILE(selinux/selinux.h HAVE_SELINUX_SELINUX_H)
CHECK_INCLUDE_FILE(proc/devname.h HAVE_PROC_DEVNAME_H)
CHECK_INCLUDE_FILE(shadow.h HAVE_SHADOW_H)
CHECK_INCLUDE_FILE(sys/systeminfo.h HAVE_SYS_SYSTEMINFO_H)
CHECK_INCLUDE_FILE(acl/libacl.h HAVE_ACL_LIBACL_H)
CHECK_INCLUDE_FILE(sys/acl.h HAVE_SYS_ACL_H)

# HAVE_ATOMIC_BUILTINS
include(CheckCSourceCompiles)
CHECK_C_SOURCE_COMPILES("#include <stdint.h>\nint main() {uint16_t foovar=0; uint16_t old=1; uint16_t new=2;__sync_bool_compare_and_swap(&foovar,old,new); return __sync_fetch_and_add(&foovar, 1); __sync_fetch_and_add(&foovar, 1);}" HAVE_ATOMIC_BUILTINS)
if(NOT HAVE_ATOMIC_BUILTINS)
    message(WARNING "!!! Compiler does not support atomic builtins. Atomic operation will be emulated using mutex-based locking. !!!")
endif()

# HAVE VALGRIND AND ENV
mark_as_advanced(ENV_PRESENT VALGRIND_PRESENT)
find_program(ENV_PRESENT env)
find_program(VALGRIND_PRESENT valgrind)

# ---------- SWITCHES

# ENABLES CCE
set(ENABLE_CCE FALSE CACHE BOOL "enables CCE")

# ENABLES PYTHON
set(ENABLE_PYTHON TRUE CACHE BOOL "enables Python")

# ENABLES PYTHON3
set(ENABLE_PYTHON3 TRUE CACHE BOOL "enables Python3")

# ENABLES PERL
set(ENABLE_PERL FALSE CACHE BOOL "enables Perl")

# ENABLES VALGRIND
set(ENABLE_VALGRIND FALSE CACHE BOOL "enables Valgrind")

# ENABLES SSP
set(ENABLE_SSP FALSE CACHE BOOL "enables fstack-protector")

# ENABLES SCE
set(ENABLE_SCE FALSE CACHE BOOL "enables SCE")

# ENABLES PROBES
set(ENABLE_PROBES TRUE CACHE BOOL "enables probes")

# ENABLES INDEPENDENT PROBES
set(ENABLE_PROBES_INDEPENDENT TRUE CACHE BOOL "enables independent probes")

# ENABLES UNIX PROBES
set(ENABLE_PROBES_UNIX TRUE CACHE BOOL "enables unix probes")

# ENABLES LINUX PROBES
STRING(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Linux" IS_LINUX)
set(ENABLE_PROBES_LINUX ${IS_LINUX} CACHE BOOL "enables linux probes")

# ENABLES SOLARIS PROBES
STRING(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Solaris" IS_SOLARIS)
set(ENABLE_PROBES_SOLARIS ${IS_SOLARIS} CACHE BOOL "enables solaris probes")

# ENABLES OSCAP UTIL
set(ENABLE_OSCAP_UTIL TRUE CACHE BOOL "enables oscap util")

# ENABLES OSCAP UTIL AS RPM
set(ENABLE_OSCAP_UTIL_AS_RPM TRUE CACHE BOOL "enables oscap util as RPM")

# ENABLES OSCAP UTIL DOCKER
set(ENABLE_OSCAP_UTIL_DOCKER TRUE CACHE BOOL "enables oscap util Docker")
if(ENABLE_OSCAP_UTIL_DOCKER AND NOT BZIP2_FOUND)
    set(ENABLE_OSCAP_UTIL_DOCKER FALSE)
    message(SEND_ERROR "oscap-docker requires bzip2! Either disable oscap-docker or install bzip2")
endif()

# ENABLES OSCAP UTIL SSH
set(ENABLE_OSCAP_UTIL_SSH TRUE CACHE BOOL "enables oscap util SSH")

# ENABLES OSCAP UTIL VM
set(ENABLE_OSCAP_UTIL_VM TRUE CACHE BOOL "enables oscap util VM utility")

# ENABLES OSCAP UTIL CHROOT
set(ENABLE_OSCAP_UTIL_CHROOT TRUE CACHE BOOL "enables oscap util chroot")

# SEAP_MSGID_BITS
set(SEAP_MSGID_BITS 32 CACHE STRING "Size of SEAP_msgid_t in bits [32|64]")

# WANT BASE64
set(WANT_BASE64 TRUE CACHE BOOL "wants builtin Base64")

# WANT XBASE64
set(WANT_XBASE64 FALSE CACHE BOOL "wants builtin XBase64")

# ---------- CONFIGURATION

configure_file("config.h.in" "config.h")
add_definitions(-DHAVE_CONFIG_H)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -std=c99 -W -Wall -Wnonnull -Wshadow -Wformat -Wundef -Wno-unused-parameter -Wmissing-prototypes -Wno-unknown-pragmas -D_GNU_SOURCE -D_POSIX_C_SOURCE=200112L")
if(${CMAKE_SYSTEM_NAME} EQUAL "Solaris")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__EXTENSIONS__")
endif()

include_directories(
    "src/"
    "src/common/"
    "src/common/public/"
    "src/CCE/public/"
    "src/CPE/public/"
    "src/CVE/public/"
    "src/CVSS/public/"
    "src/DS/public/"
    "src/OVAL/public/"
    "src/OVAL/probes/SEAP/"
    "src/OVAL/probes/SEAP/public/"
    "src/OVAL/"
    "src/source/public/"
    "src/XCCDF/"
    "src/XCCDF/public/"
    "src/XCCDF_POLICY/"
    "src/XCCDF_POLICY/public/"
    ${CMAKE_BINARY_DIR}
    ${XML2_INCLUDE_DIRS}
)

add_subdirectory("src")
add_subdirectory("utils")
