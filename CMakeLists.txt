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

# ---------- INCLUDES CMAKE MODULES

include(GNUInstallDirs)
include(FindPkgConfig)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CPack)

# ---------- DEPENDENCIES

# ACL
find_package(ACL)
if(ACL_FOUND)
	CHECK_LIBRARY_EXISTS(acl acl_extended_file "" HAVE_ACL_EXTENDED_FILE)
	CHECK_INCLUDE_FILE(acl/libacl.h HAVE_ACL_LIBACL_H)
	CHECK_INCLUDE_FILE(sys/acl.h HAVE_SYS_ACL_H)
endif()

# APTPKG
find_package(AptPkg)

# BLKID
find_package(Blkid)
if(BLKID_FOUND)
    CHECK_LIBRARY_EXISTS(blkid blkid_get_tag_value "" HAVE_BLKID_GET_TAG_VALUE)
endif()

# CAP
find_package(Cap)
if(CAP_FOUND)
	CHECK_LIBRARY_EXISTS(cap cap_get_pid "" HAVE_CAP_GET_PID)
	CHECK_INCLUDE_FILE(sys/capability.h HAVE_SYS_CAPABILITY_H)
endif()

# CURL
find_package(CURL)

# DBUS
find_package(DBus)

# DOXYGEN
find_package(Doxygen)

# GCONF2
find_package(GConf)

# LDAP
find_package(Ldap)

# OpenDbx
find_package(OpenDbx)

# PCRE
find_package(PCRE REQUIRED)

# PERL
find_package(PerlLibs)

# POPT
find_package(Popt)

# PROCPS
find_package(Procps)
if(PROCPS_FOUND)
    CHECK_LIBRARY_EXISTS(procps dev_to_tty "" HAVE_DEV_TO_TTY)
	CHECK_INCLUDE_FILE(proc/devname.h HAVE_PROC_DEVNAME_H)
endif()

# PYTHON
find_package(PythonLibs)
find_package(PythonInterp)

# RPM
find_package(RPM)
if(RPM_FOUND)
    CHECK_LIBRARY_EXISTS(rpm rpmReadConfigFiles "" HAVE_RPMREADCONFIGFILES)
    CHECK_LIBRARY_EXISTS(rpm headerFormat "" HAVE_HEADERFORMAT)
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
find_package(SELinux)
if(SELINUX_FOUND)
	CHECK_INCLUDE_FILE(selinux/selinux.h HAVE_SELINUX_SELINUX_H)
endif()

# SWIG
find_package(SWIG)

# XML2
find_package(LibXml2 REQUIRED)

# XSLT
find_package(LibXslt REQUIRED)

# BZip2
find_package(BZip2)

# PThread
find_package(Threads)
set(CMAKE_THREAD_PREFER_PTHREAD)
set(THREADS_PREFER_PTHREAD_FLAG)
CHECK_LIBRARY_EXISTS(pthread pthread_timedjoin_np "" HAVE_PTHREAD_TIMEDJOIN_NP)
CHECK_LIBRARY_EXISTS(pthread pthread_setname_np "" HAVE_PTHREAD_SETNAME_NP)
CHECK_LIBRARY_EXISTS(pthread pthread_getname_np "" HAVE_PTHREAD_GETNAME_NP)

# WITH_CRYPTO
set(WITH_CRYPTO "gcrypt" CACHE STRING "gcrypt|nss3")
if(NOT (${WITH_CRYPTO} EQUAL "nss3"))
	# gcrypt
	find_package(GCrypt)
else()
	# nss3
	find_package(NSS)
endif()

# HAVE FUNCTIONS
CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_CLOCK_GETTIME)
CHECK_FUNCTION_EXISTS(posix_memalign HAVE_POSIX_MEMALIGN)
CHECK_FUNCTION_EXISTS(memalign HAVE_MEMALIGN)

# HAVE HEADERS
CHECK_INCLUDE_FILE(syslog.h HAVE_SYSLOG_H)
CHECK_INCLUDE_FILE(stdio_ext.h HAVE_STDIO_EXT_H)
CHECK_INCLUDE_FILE(shadow.h HAVE_SHADOW_H)
CHECK_INCLUDE_FILE(sys/systeminfo.h HAVE_SYS_SYSTEMINFO_H)

# HAVE_ATOMIC_BUILTINS
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
set(OSCAP_DOCKER_PYTHON ${PYTHON_EXECUTABLE} CACHE STRING "Path to the Python interpreter for oscap-docker")
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

# ---------- PATHS

set(OSCAP_DEFAULT_SCHEMA_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/schemas")
set(OSCAP_DEFAULT_XSLT_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/xsl")
set(OSCAP_DEFAULT_CPE_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/cpe")
set(OVAL_PROBE_DIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/openscap")
set(OVAL_PROBEDIR_ENV 1)

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
	${LIBXML2_INCLUDE_DIR}
)

add_subdirectory("src")
add_subdirectory("utils")
add_subdirectory("docs")
add_subdirectory("dist")
add_subdirectory("schemas")
add_subdirectory("xsl")
add_subdirectory("cpe")
add_subdirectory("swig")
configure_file("run.in" "run" @ONLY)

# pkgconfig file
configure_file("libopenscap.pc.in" "libopenscap.pc" @ONLY)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/libopenscap.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# CPack
set(CPACK_SOURCE_PACKAGE_FILE_NAME "openscap-${OPENSCAP_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
# TODO: remove ac_probes, config and m4 directories after automake will be removed
set(CPACK_SOURCE_IGNORE_FILES
	"\\\\.git.*"
	"ac_probes/"
	"config/"
	"build/"
	"m4/"
	"~$"
	"\\\\CMakeLists.txt.user"
)

# Ctest
enable_testing()
add_subdirectory("tests")
