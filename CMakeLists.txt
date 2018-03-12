cmake_minimum_required(VERSION 2.6)

project("openscap")
set(OPENSCAP_VERSION_MAJOR "1")
set(OPENSCAP_VERSION_MINOR "3")
set(OPENSCAP_VERSION_PATCH "0")
set(OPENSCAP_VERSION "${OPENSCAP_VERSION_MAJOR}.${OPENSCAP_VERSION_MINOR}.${OPENSCAP_VERSION_PATCH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

message(STATUS "OpenSCAP ${OPENSCAP_VERSION}")
message(STATUS "(see ${CMAKE_SOURCE_DIR}/README.md for build instructions)")
message(STATUS " ")

# Strictly speaking in-source will work but will be very messy, let's
# discourage our users from using them
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "In-source builds are not supported! Please use out of source builds:\n"
		"$ cd openscap\n"
		"$ rm CMakeCache.txt\n"
		"$ cd build\n"
		"$ cmake ../\n"
		"$ make -j4"
		)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(NDEBUG 1)
endif()

# In Microsoft Visual Studio, store built binaries to a single directory.
# We need to build all the binaries in a single directory on Windows, because
# vcpkg tool fails to fetch dependent DLLs (libxml, libcurl, etc.) if
# oscap.exe is build to a different directory than openscap.dll.
# See the discussion in https://github.com/Microsoft/vcpkg/issues/1002
if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# ---------- INCLUDES CMAKE MODULES

include(GNUInstallDirs)
include(FindPkgConfig)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)

# ---------- DEPENDENCIES

find_package(ACL)
if(ACL_FOUND)
	check_library_exists(acl acl_extended_file "" HAVE_ACL_EXTENDED_FILE)
	check_include_file(acl/libacl.h HAVE_ACL_LIBACL_H)
	check_include_file(sys/acl.h HAVE_SYS_ACL_H)
endif()

find_package(AptPkg)

find_package(Blkid)
if(BLKID_FOUND)
	check_library_exists(blkid blkid_get_tag_value "" HAVE_BLKID_GET_TAG_VALUE)
endif()

find_package(Cap)
if(CAP_FOUND)
	check_library_exists(cap cap_get_pid "" HAVE_CAP_GET_PID)
endif()

find_package(CURL)
find_package(DBus)
find_package(Doxygen)
find_package(GConf)
find_package(Ldap)
find_package(OpenDbx)
find_package(PCRE REQUIRED)
find_package(PerlLibs)
find_package(Popt)

find_package(Procps)
if(PROCPS_FOUND)
	check_library_exists(procps dev_to_tty "" HAVE_DEV_TO_TTY)
	check_include_file(proc/devname.h HAVE_PROC_DEVNAME_H)
endif()

find_package(PythonLibs 2 EXACT)
find_package(PythonInterp)

find_package(RPM)
if(RPM_FOUND)
	check_library_exists(rpm rpmReadConfigFiles "" HAVE_RPMREADCONFIGFILES)
	check_library_exists(rpm headerFormat "" HAVE_HEADERFORMAT)
	check_library_exists(rpmio rpmFreeCrypto "" HAVE_RPMFREECRYPTO)
	check_library_exists(rpm rpmFreeFilesystems "" HAVE_RPMFREEFILESYSTEMS)
	set(HAVE_RPMVERCMP 1)
endif()

find_package(SELinux)
find_package(SWIG)
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
find_package(BZip2)

# PThread
set(CMAKE_THREAD_PREFER_PTHREAD)
set(THREADS_PREFER_PTHREAD_FLAG)
set(THREADS_USE_PTHREADS_WIN32 true)
find_package(Threads REQUIRED)
check_library_exists(pthread pthread_timedjoin_np "" HAVE_PTHREAD_TIMEDJOIN_NP)
check_library_exists(pthread pthread_setname_np "" HAVE_PTHREAD_SETNAME_NP)
check_library_exists(pthread pthread_getname_np "" HAVE_PTHREAD_GETNAME_NP)

# WITH_CRYPTO
set(WITH_CRYPTO "gcrypt" CACHE STRING "gcrypt|nss3")
if(NOT (${WITH_CRYPTO} EQUAL "nss3"))
	# gcrypt
	find_package(GCrypt)
else()
	# nss3
	find_package(NSS)
endif()

check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(memalign HAVE_MEMALIGN)
check_function_exists(fts_open HAVE_FTS_OPEN)
check_function_exists(strsep HAVE_STRSEP)
check_function_exists(strptime HAVE_STRPTIME)

check_include_file(syslog.h HAVE_SYSLOG_H)
check_include_file(stdio_ext.h HAVE_STDIO_EXT_H)
check_include_file(shadow.h HAVE_SHADOW_H)
check_include_file(sys/systeminfo.h HAVE_SYS_SYSTEMINFO_H)
check_include_file(getopt.h HAVE_GETOPT_H)
check_include_file(sys/uio.h HAVE_UIO_H)

# HAVE_ATOMIC_BUILTINS
check_c_source_compiles("#include <stdint.h>\nint main() {uint16_t foovar=0; uint16_t old=1; uint16_t new=2;__sync_bool_compare_and_swap(&foovar,old,new); return __sync_fetch_and_add(&foovar, 1); __sync_fetch_and_add(&foovar, 1);}" HAVE_ATOMIC_BUILTINS)
if(NOT HAVE_ATOMIC_BUILTINS)
	message(WARNING "!!! Compiler does not support atomic builtins. Atomic operation will be emulated using mutex-based locking. !!!")
endif()

mark_as_advanced(ENV_PRESENT VALGRIND_PRESENT)
find_program(ENV_PRESENT env)
find_program(VALGRIND_PRESENT valgrind)

# ---------- CORE FEATURE SWITCHES

option(ENABLE_CCE "enables support for Common Configuration Enumeration" FALSE)
option(ENABLE_SCE "enables Script Check Engine - an alternative checking engine that let's you use executables instead of OVAL for checks" FALSE)

# ---------- OVAL FEATURE SWITCHES

option(ENABLE_PROBES "build OVAL probes - each probe implements an OVAL test" TRUE)
set(SEAP_MSGID_BITS 32 CACHE STRING "Size of SEAP_msgid_t in bits [32|64]")
option(ENABLE_PROBES_INDEPENDENT "build OVAL probes for independent (cross platform) OVAL tests" TRUE)
option(ENABLE_PROBES_UNIX "build OVAL probes for the UNIX OVAL tests" ${UNIX})
string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Linux" IS_LINUX)
option(ENABLE_PROBES_LINUX "build OVAL probes for the Linux OVAL tests" ${IS_LINUX})
string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Solaris" IS_SOLARIS)
option(ENABLE_PROBES_SOLARIS "build OVAL probes for the Solaris OVAL tests" ${IS_SOLARIS})

if(ENABLE_PROBES)
	if(ENABLE_PROBES_INDEPENDENT)
		set(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE TRUE)
		set(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE58 TRUE)
		set(OPENSCAP_PROBE_INDEPENDENT_FAMILY TRUE)
		if(GCRYPT_FOUND OR NSS_FOUND)
			set(OPENSCAP_PROBE_INDEPENDENT_FILEHASH TRUE)
			set(OPENSCAP_PROBE_INDEPENDENT_FILEHASH58 TRUE)
		endif()
		if(OPENDBX_FOUND)
			set(OPENSCAP_PROBE_INDEPENDENT_SQL TRUE)
			set(OPENSCAP_PROBE_INDEPENDENT_SQL57 TRUE)
		endif()
		set(OPENSCAP_PROBE_INDEPENDENT_SYSTEM_INFO TRUE)
		set(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT TRUE)
		set(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT54 TRUE)
		set(OPENSCAP_PROBE_INDEPENDENT_VARIABLE TRUE)
		set(OPENSCAP_PROBE_INDEPENDENT_XMLFILECONTENT TRUE)
	endif()
	if(ENABLE_PROBES_UNIX)
		set(OPENSCAP_PROBE_UNIX_DNSCACHE TRUE)
		set(OPENSCAP_PROBE_UNIX_FILE TRUE)
		set(OPENSCAP_PROBE_UNIX_FILEEXTENDEDATTRIBUTE TRUE)
		if(GCONF_FOUND)
			set(OPENSCAP_PROBE_UNIX_GCONF TRUE)
		endif()
		set(OPENSCAP_PROBE_UNIX_INTERFACE TRUE)
		set(OPENSCAP_PROBE_UNIX_PASSWORD TRUE)
		set(OPENSCAP_PROBE_UNIX_PROCESS TRUE)
		if(CAP_FOUND)
			set(OPENSCAP_PROBE_UNIX_PROCESS58 TRUE)
		endif()
		set(OPENSCAP_PROBE_UNIX_ROUTINGTABLE TRUE)
		set(OPENSCAP_PROBE_UNIX_RUNLEVEL TRUE)
		set(OPENSCAP_PROBE_UNIX_SHADOW TRUE)
		set(OPENSCAP_PROBE_UNIX_SYMLINK TRUE)
		set(OPENSCAP_PROBE_UNIX_SYSCTL TRUE)
		set(OPENSCAP_PROBE_UNIX_UNAME TRUE)
		set(OPENSCAP_PROBE_UNIX_XINETD TRUE)
		if(ENABLE_PROBES_LINUX)
			if(APTPKG_FOUND)
				set(OPENSCAP_PROBE_LINUX_DPKGINFO TRUE)
			endif()
			set(OPENSCAP_PROBE_LINUX_IFLISTENERS TRUE)
			set(OPENSCAP_PROBE_LINUX_INETLISTENINGSERVERS TRUE)
			if(BLKID_FOUND)
				set(OPENSCAP_PROBE_LINUX_PARTITION TRUE)
			endif()
			if(RPM_FOUND)
				set(OPENSCAP_PROBE_LINUX_RPMINFO TRUE)
				set(OPENSCAP_PROBE_LINUX_RPMVERIFY TRUE)
				set(OPENSCAP_PROBE_LINUX_RPMVERIFYFILE TRUE)
				set(OPENSCAP_PROBE_LINUX_RPMVERIFYPACKAGE TRUE)
			endif()
			if(SELINUX_FOUND)
				set(OPENSCAP_PROBE_LINUX_SELINUXBOOLEAN TRUE)
				set(OPENSCAP_PROBE_LINUX_SELINUXSECURITYCONTEXT TRUE)
			endif()
			if(DBUS_FOUND)
				set(OPENSCAP_PROBE_LINUX_SYSTEMDUNITDEPENDENCY TRUE)
				set(OPENSCAP_PROBE_LINUX_SYSTEMDUNITPROPERTY TRUE)
			endif()
		endif()
	endif()
endif()

# ---------- EXECUTABLES / UTILITIES SWITCHES

option(ENABLE_OSCAP_UTIL "build the oscap utility, oscap is the core utility for evaluation and processing of SCAP data" TRUE)
option(ENABLE_OSCAP_UTIL_AS_RPM "enable the scap-as-rpm utility, this let's you package SCAP data as RPMs" TRUE)

set(OSCAP_DOCKER_PYTHON ${PYTHON_EXECUTABLE} CACHE STRING "path to the Python interpreter for oscap-docker")
option(ENABLE_OSCAP_UTIL_DOCKER "enables the oscap-docker utility, this let's you scan containers and container images" TRUE)
if(ENABLE_OSCAP_UTIL_DOCKER AND NOT BZIP2_FOUND)
	set(ENABLE_OSCAP_UTIL_DOCKER FALSE)
	message(SEND_ERROR "oscap-docker requires bzip2! Either disable oscap-docker or install bzip2")
endif()

option(ENABLE_OSCAP_UTIL_SSH "enables the oscap-ssh utility, this let's you scan remote machines over ssh" TRUE)
option(ENABLE_OSCAP_UTIL_VM "enables the oscap-vm utility, this let's you scan VMs and VM storage images" TRUE)
option(ENABLE_OSCAP_UTIL_CHROOT "enables the oscap-chroot utility, this let's you scan entire chroots using offline scanning" TRUE)

# ---------- TEST-SUITE SWITCHES

# Tests will be turned off on Windows, because the test suite uses bash
# and other Linux-specific tools.
if(WIN32)
	# TODO: I hate that the doc string is duplicated but cmake doesn't support evaluating expressions :-/
	option(ENABLE_TESTS "enables the test suite, use `ctest` to run it" FALSE)
else()
	option(ENABLE_TESTS "enables the test suite, use `ctest` to run it" TRUE)
endif()

option(ENABLE_VALGRIND "enables Valgrind memory testing in the test-suite" FALSE)

# ---------- LANGUAGE BINDINGS

# TODO: Shouldn't we only enable this if python2 was found?
option(ENABLE_PYTHON2 "if enabled, the python2 swig bindings will be built" TRUE)

# TODO: Shouldn't we only enable this if python3 was found?
option(ENABLE_PYTHON3 "if enabled, the python3 swig bindings will be built" FALSE)

# TODO: Shouldn't we only enable this if perl was found?
option(ENABLE_PERL "if enabled, the perl swig bindings will be built" FALSE)

# ---------- NO IDEA WHAT THIS IS FOR
set(WANT_BASE64 TRUE CACHE BOOL "wants builtin Base64")
set(WANT_XBASE64 FALSE CACHE BOOL "wants builtin XBase64")


message(STATUS " ")
message(STATUS "CMake:")
message(STATUS "generator: ${CMAKE_GENERATOR}")
message(STATUS "source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "build directory: ${CMAKE_BINARY_DIR}")
message(STATUS " ")

message(STATUS "Core features:")
message(STATUS "CCE: ${ENABLE_CCE}")
message(STATUS "SCE: ${ENABLE_SCE}")
message(STATUS " ")

message(STATUS "OVAL:")
message(STATUS "base probe support: ${ENABLE_PROBES}")
message(STATUS "SEAP msgid bit-size: ${SEAP_MSGID_BITS}")
message(STATUS "independent probes: ${ENABLE_PROBES_INDEPENDENT}")
message(STATUS "unix probes: ${ENABLE_PROBES_UNIX}")
message(STATUS "linux probes: ${ENABLE_PROBES_LINUX}")
message(STATUS "solaris probes: ${ENABLE_PROBES_SOLARIS}")
message(STATUS " ")

message(STATUS "Language bindings:")
message(STATUS "python2 bindings: ${ENABLE_PYTHON2}")
message(STATUS "python3 bindings: ${ENABLE_PYTHON3}")
message(STATUS "perl bindings: ${ENABLE_PERL}")
message(STATUS " ")

# ---------- PATHS

set(OSCAP_DEFAULT_SCHEMA_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/schemas")
set(OSCAP_DEFAULT_XSLT_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/xsl")
set(OSCAP_DEFAULT_CPE_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/cpe")
set(OVAL_PROBE_DIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/openscap")
set(OVAL_PROBEDIR_ENV 1)

# ---------- RPATHS for linking

# see https://cmake.org/Wiki/CMake_RPATH_handling

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# ---------- CONFIGURATION

configure_file("config.h.in" "config.h")
add_definitions(-DHAVE_CONFIG_H)
if (MSVC)
	# Disable some of Microsoft Visual Studio 2017 warnings
	#
	# Visual Studio recommends using some non-standard functions with _s suffix
	# instead of standard functions, because they considered it more secure.
	# However these functions are available only in Microsoft C Runtime.
	# Therefore we disable this type of warnings.
	# https://docs.microsoft.com/en-us/cpp/c-runtime-library/security-features-in-the-crt
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	# Microsoft has renamed some POSIX functions in the CRT to conform with C99 rules for
	# implementation-defined global function names. In most cases, a leading underscore was
	# added to the POSIX function name to create a standard conformant name.
	# If we use POSIX functions without leading underscore, a deprecation warning is shown.
	# Therefore we disable this type of warnings.
	# https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996
	add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -std=c99 -W -Wall -Wnonnull -Wshadow -Wformat -Wundef -Wno-unused-parameter -Wmissing-prototypes -Wno-unknown-pragmas -D_GNU_SOURCE -D_POSIX_C_SOURCE=200112L")
endif()
if(${CMAKE_SYSTEM_NAME} EQUAL "Solaris")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__EXTENSIONS__")
endif()
if(WIN32)
	# expose new WinAPI function appearing on Windows 7
	# eg. inet_pton
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WIN32_WINNT=0x0600")
endif()

include_directories(
	"compat/"
	"src/"
	"src/common/"
	"src/common/public/"
	"src/CCE/public/"
	"src/CPE/public/"
	"src/CVE/public/"
	"src/CVRF/public/"
	"src/CVSS/public/"
	"src/DS/public/"
	"src/OVAL/public/"
	"src/OVAL/probes/public/"
	"src/OVAL/probes/SEAP/"
	"src/OVAL/probes/SEAP/public/"
	"src/OVAL/"
	"src/source/public/"
	"src/XCCDF/"
	"src/XCCDF/public/"
	"src/XCCDF_POLICY/"
	"src/XCCDF_POLICY/public/"
	${CMAKE_BINARY_DIR} # config.h is generated to build directory
	${LIBXML2_INCLUDE_DIR}
)

add_subdirectory("compat")
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

# Ctest
if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory("tests")
endif()

# CPack
set(CPACK_SOURCE_PACKAGE_FILE_NAME "openscap-${OPENSCAP_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
	"\\\\.git.*"
	"build/"
	"~$"
	"\\\\CMakeLists.txt.user"
)
include(CPack)
