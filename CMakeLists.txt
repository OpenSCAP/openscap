cmake_minimum_required(VERSION 2.6)

# Inspired and referenced from https://blog.kitware.com/cmake-and-the-default-build-type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
		"MinSizeRel" "RelWithDebInfo")
endif()

project("openscap")
set(OPENSCAP_VERSION_MAJOR "1")
set(OPENSCAP_VERSION_MINOR "3")
#TODO: Remove this before final 1.3.0
#FIXME: Windows Installer allows only integers in version name.
if(WIN32)
	set(OPENSCAP_VERSION_PATCH "0")
else()
	set(OPENSCAP_VERSION_PATCH "0_alpha2")
endif()
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
include(CMakeDependentOption)

# ---------- DEPENDENCIES

find_package(ACL)
if(ACL_FOUND)
	check_library_exists("${ACL_LIBRARY}" acl_extended_file "" HAVE_ACL_EXTENDED_FILE)
	check_include_file(acl/libacl.h HAVE_ACL_LIBACL_H)
	check_include_file(sys/acl.h HAVE_SYS_ACL_H)
endif()

find_package(AptPkg)

find_package(Blkid)
if(BLKID_FOUND)
	check_library_exists("${BLKID_LIBRARY}" blkid_get_tag_value "" HAVE_BLKID_GET_TAG_VALUE)
endif()

find_package(Cap)
if(CAP_FOUND)
	check_library_exists("${CAP_LIBRARIES}" cap_get_pid "" HAVE_CAP_GET_PID)
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
	check_library_exists("${PROCPS_LIBRARY}" dev_to_tty "" HAVE_DEV_TO_TTY)
	check_include_file(proc/devname.h HAVE_PROC_DEVNAME_H)
endif()

# According to CMake documentation:
# https://cmake.org/cmake/help/latest/module/FindPythonInterp.html
# If calling both find_package(PythonInterp) and find_package(PythonLibs),
# call find_package(PythonInterp) first to get the currently active Python
# version by default with a consistent version of PYTHON_LIBRARIES.
find_package(PythonInterp 3)
find_package(PythonLibs 3)

find_package(RPM)
if(RPM_FOUND)
	check_library_exists("${RPM_LIBRARY}" rpmReadConfigFiles "" HAVE_RPMREADCONFIGFILES)
	check_library_exists("${RPM_LIBRARY}" headerFormat "" HAVE_HEADERFORMAT)
	check_library_exists("${RPMIO_LIBRARY}" rpmFreeCrypto "" HAVE_RPMFREECRYPTO)
	check_library_exists("${RPM_LIBRARY}" rpmFreeFilesystems "" HAVE_RPMFREEFILESYSTEMS)
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
if(GCRYPT_FOUND OR NSS_FOUND)
	set(CRYPTO_FOUND TRUE)
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
check_include_file(sys/xattr.h HAVE_SYS_XATTR_H)
check_include_file(attr/xattr.h HAVE_ATTR_XATTR_H)

# HAVE_ATOMIC_BUILTINS
check_c_source_compiles("#include <stdint.h>\nint main() {uint16_t foovar=0; uint16_t old=1; uint16_t new=2;__sync_bool_compare_and_swap(&foovar,old,new); return __sync_fetch_and_add(&foovar, 1); __sync_fetch_and_add(&foovar, 1);}" HAVE_ATOMIC_BUILTINS)
if(NOT HAVE_ATOMIC_BUILTINS)
	message(WARNING "!!! Compiler does not support atomic builtins. Atomic operation will be emulated using mutex-based locking. !!!")
endif()

mark_as_advanced(ENV_PRESENT VALGRIND_PRESENT)
find_program(ENV_PRESENT env)
find_program(VALGRIND_PRESENT valgrind)
find_program(ASCIIDOCTOR_EXECUTABLE NAMES asciidoc asciidoctor)
find_program(SED_EXECUTABLE sed)
find_program(GIT_EXECUTABLE git)

# ---------- CORE FEATURE SWITCHES

option(ENABLE_CCE "enables support for Common Configuration Enumeration" FALSE)
option(ENABLE_SCE "enables Script Check Engine - an alternative checking engine that let's you use executables instead of OVAL for checks" FALSE)

# ---------- OVAL FEATURE SWITCHES

option(ENABLE_PROBES "build OVAL probes - each probe implements an OVAL test" TRUE)
set(SEAP_MSGID_BITS 32 CACHE STRING "Size of SEAP_msgid_t in bits [32|64]")
cmake_dependent_option(ENABLE_PROBES_INDEPENDENT "build OVAL probes for independent (cross platform) OVAL tests" ON "ENABLE_PROBES" OFF)
# On some platforms (Windows..) UNIX ends up being empty instead of "false"
set(IS_UNIX FALSE)
if (UNIX)
    set(IS_UNIX TRUE)
endif()
cmake_dependent_option(ENABLE_PROBES_UNIX "build OVAL probes for the UNIX OVAL tests" ${IS_UNIX} "ENABLE_PROBES" OFF)
string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Linux" IS_LINUX)
cmake_dependent_option(ENABLE_PROBES_LINUX "build OVAL probes for the Linux OVAL tests" ${IS_LINUX} "ENABLE_PROBES" OFF)
string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Solaris" IS_SOLARIS)
cmake_dependent_option(ENABLE_PROBES_SOLARIS "build OVAL probes for the Solaris OVAL tests" ${IS_SOLARIS} "ENABLE_PROBES" OFF)
set(IS_WIN32 FALSE)
if (WIN32)
    set(IS_WIN32 TRUE)
endif()
cmake_dependent_option(ENABLE_PROBES_WINDOWS "build OVAL probes for the Windows OVAL tests" ${IS_WIN32} "ENABLE_PROBES" OFF)

# INDEPENDENT PROBES
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE "Independent environmentvariable probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE58 "Independent environmentvariable58 probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_FAMILY "Independent family probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_FILEHASH "Independent filehash probe" ON "ENABLE_PROBES_INDEPENDENT; CRYPTO_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_FILEHASH58 "Independent filehash58 probe" ON "ENABLE_PROBES_INDEPENDENT; CRYPTO_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_SQL "Independent sql probe" ON "ENABLE_PROBES_INDEPENDENT; OPENDBX_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_SQL57 "Independent sql57 probe" ON "ENABLE_PROBES_INDEPENDENT; OPENDBX_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_SYSTEM_INFO "Independent system info probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT "Independent textfilecontent probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT54 "Independent textfilecontent54 probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_VARIABLE "Independent variable probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_XMLFILECONTENT "Independent xmlfilecontent probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)

# UNIX PROBES
cmake_dependent_option(OPENSCAP_PROBE_UNIX_DNSCACHE "Unix dnscache probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_FILE "Unix file probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_FILEEXTENDEDATTRIBUTE "Unix fileextendedattribute probe" ON "ENABLE_PROBES_UNIX; (HAVE_SYS_XATTR_H OR HAVE_ATTR_XATTR_H)" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_GCONF "Unix gconf probe" ON "ENABLE_PROBES_UNIX; GCONF_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_INTERFACE "Unix interface probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_PASSWORD "Unix password probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_PROCESS "Unix process probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_PROCESS58 "Unix process58 probe" ON "ENABLE_PROBES_UNIX; CAP_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_ROUTINGTABLE "Unix routingtable probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_RUNLEVEL "Unix runlevel probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_SHADOW "Unix shadow probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_SYMLINK "Unix symlink probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_SYSCTL "Unix sysctl probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_UNAME "Unix uname probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_XINETD "Unix xinetd probe" ON "ENABLE_PROBES_UNIX" OFF)

# LINUX PROBES
cmake_dependent_option(OPENSCAP_PROBE_LINUX_DPKGINFO "Linux dpkginfo probe" ON "ENABLE_PROBES_LINUX; APTPKG_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_IFLISTENERS "Linux iflisteners probe" ON "ENABLE_PROBES_LINUX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_INETLISTENINGSERVERS "Linux inetlisteningservers probe" ON "ENABLE_PROBES_LINUX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_PARTITION "Linux partition probe" ON "ENABLE_PROBES_LINUX; BLKID_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_RPMINFO "Linux rpminfo probe" ON "ENABLE_PROBES_LINUX; RPM_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_RPMVERIFY "Linux rpmverify probe" ON "ENABLE_PROBES_LINUX; RPM_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_RPMVERIFYFILE "Linux rpmverifyfile probe" ON "ENABLE_PROBES_LINUX; RPM_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_RPMVERIFYPACKAGE "Linux rpmverifypackage probe" ON "ENABLE_PROBES_LINUX; RPM_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_SELINUXBOOLEAN "Linux selinuxboolean probe" ON "ENABLE_PROBES_LINUX; SELINUX_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_SELINUXSECURITYCONTEXT "Linux selinuxsecuritycontext probe" ON "ENABLE_PROBES_LINUX; SELINUX_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_SYSTEMDUNITDEPENDENCY "Linux systemdunitdependency probe" ON "ENABLE_PROBES_LINUX; DBUS_FOUND" OFF)
cmake_dependent_option(OPENSCAP_PROBE_LINUX_SYSTEMDUNITPROPERTY "Linux systemdunitproperty probe" ON "ENABLE_PROBES_LINUX; DBUS_FOUND" OFF)

# SOLARIS PROBES
cmake_dependent_option(OPENSCAP_PROBE_SOLARIS_ISAINFO "Solaris isainfo probe" ON "ENABLE_PROBES_SOLARIS" OFF)

# WINDOWS PROBES
cmake_dependent_option(OPENSCAP_PROBE_WINDOWS_ACCESSTOKEN "Windows accesstoken probe" ON "ENABLE_PROBES_WINDOWS" OFF)
cmake_dependent_option(OPENSCAP_PROBE_WINDOWS_REGISTRY "Windows registry probe" ON "ENABLE_PROBES_WINDOWS" OFF)
cmake_dependent_option(OPENSCAP_PROBE_WINDOWS_WMI57 "Windows wmi57 probe" ON "ENABLE_PROBES_WINDOWS" OFF)


# ---------- EXECUTABLES / UTILITIES SWITCHES

option(ENABLE_OSCAP_UTIL "build the oscap utility, oscap is the core utility for evaluation and processing of SCAP data" TRUE)

set(OSCAP_DOCKER_PYTHON ${PYTHON_EXECUTABLE} CACHE STRING "path to the Python interpreter for oscap-docker")
cmake_dependent_option(ENABLE_OSCAP_UTIL_DOCKER "enables the oscap-docker utility, this let's you scan containers and container images" ON "NOT WIN32" OFF)
if(ENABLE_OSCAP_UTIL_DOCKER AND NOT BZIP2_FOUND)
	set(ENABLE_OSCAP_UTIL_DOCKER FALSE)
	message(SEND_ERROR "oscap-docker requires bzip2! Either disable oscap-docker or install bzip2")
endif()

cmake_dependent_option(ENABLE_OSCAP_UTIL_AS_RPM "enable the scap-as-rpm utility, this let's you package SCAP data as RPMs" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_SSH "enables the oscap-ssh utility, this let's you scan remote machines over ssh" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_VM "enables the oscap-vm utility, this let's you scan VMs and VM storage images" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_CHROOT "enables the oscap-chroot utility, this let's you scan entire chroots using offline scanning" ON "NOT WIN32" OFF)

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
cmake_dependent_option(ENABLE_PYTHON3 "if enabled, the python3 swig bindings will be built" ON "PYTHONINTERP_FOUND;SWIG_FOUND;PYTHONLIBS_FOUND" OFF)
cmake_dependent_option(ENABLE_PERL "if enabled, the perl swig bindings will be built" ON "PERLLIBS_FOUND;SWIG_FOUND" OFF)

# ---------- NO IDEA WHAT THIS IS FOR
set(WANT_BASE64 TRUE CACHE BOOL "wants builtin Base64")
set(WANT_XBASE64 FALSE CACHE BOOL "wants builtin XBase64")

# ---------- STATUS MESSAGES

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
message(STATUS "python3 bindings: ${ENABLE_PYTHON3}")
message(STATUS "perl bindings: ${ENABLE_PERL}")
message(STATUS " ")

message(STATUS "Utilities:")
message(STATUS "oscap: ${ENABLE_OSCAP_UTIL}")
message(STATUS "oscap-docker: ${ENABLE_OSCAP_UTIL_DOCKER}")
message(STATUS "scap-as-rpm: ${ENABLE_OSCAP_UTIL_AS_RPM}")
message(STATUS "oscap-ssh: ${ENABLE_OSCAP_UTIL_SSH}")
message(STATUS "oscap-vm: ${ENABLE_OSCAP_UTIL_VM}")
message(STATUS "oscap-chroot: ${ENABLE_OSCAP_UTIL_CHROOT}")
message(STATUS " ")

# ---------- PATHS

if(WIN32)
	# Windows installer does not allow full paths.
	# The install path can be changed by user in Windows installer.
	# We will use relative names - "schemas", "xsl" and "cpe"
	# directories will be located in the same directory as oscap.exe.
	set(OSCAP_DEFAULT_SCHEMA_PATH "schemas")
	set(OSCAP_DEFAULT_XSLT_PATH "xsl")
	set(OSCAP_DEFAULT_CPE_PATH "cpe")
else()
	set(OSCAP_DEFAULT_SCHEMA_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/schemas")
	set(OSCAP_DEFAULT_XSLT_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/xsl")
	set(OSCAP_DEFAULT_CPE_PATH "${CMAKE_INSTALL_FULL_DATADIR}/openscap/cpe")
endif()
set(OSCAP_TEMP_DIR "/tmp" CACHE STRING "use different temporary directory to execute sce scripts (default=/tmp)")

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

# Turn on RPATH for OSX for policy warning
set(CMAKE_MACOSX_RPATH ON)
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
if(APPLE)
	#full Single Unix Standard v3 (SUSv3) conformance (the Unix API)
	add_definitions(-D_DARWIN_C_SOURCE)
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

function(set_oscap_generic_properties TARGET_OBJECT)
	set_target_properties(${TARGET_OBJECT} PROPERTIES
		CXX_VISIBILITY_PRESET hidden    # Make global variables and functions HIDDEN by default.
		POSITION_INDEPENDENT_CODE ON    # Compile this object code position independent.
	)
	target_compile_definitions(${TARGET_OBJECT} PRIVATE OSCAP_BUILD_SHARED)
endfunction()

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

if(NOT WIN32)
	# pkgconfig file
	configure_file("libopenscap.pc.in" "libopenscap.pc" @ONLY)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/libopenscap.pc
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	)
endif()

# changelog
if(GIT_EXECUTABLE AND SED_EXECUTABLE)
	add_custom_target(changelog
		COMMAND "${GIT_EXECUTABLE}" log | "${SED_EXECUTABLE}" '/^commit/d\; /^Merge/d' > "${CMAKE_BINARY_DIR}/ChangeLog"
		COMMENT "Generating ChangeLog"
	)
endif()

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
	"build-win32/"
	"~$"
	"\\\\CMakeLists.txt.user"
)
set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME "OpenSCAP")
set(CPACK_PACKAGE_VENDOR "OpenSCAP Project")
set(CPACK_PACKAGE_VERSION "${OPENSCAP_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${OPENSCAP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${OPENSCAP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${OPENSCAP_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/lgpl-2.1.rtf")
set(CPACK_WIX_PATCH_FILE "${CMAKE_SOURCE_DIR}/wix_patch.xml")
set(CPACK_PACKAGE_CHECKSUM SHA512)

include(CPack)
