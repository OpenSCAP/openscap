cmake_minimum_required(VERSION 2.8...3.19)

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
set(OPENSCAP_VERSION_PATCH "12")

if(OPENSCAP_VERSION_SUFFIX)
	set(OPENSCAP_VERSION "${OPENSCAP_VERSION_MAJOR}.${OPENSCAP_VERSION_MINOR}.${OPENSCAP_VERSION_PATCH}_${OPENSCAP_VERSION_SUFFIX}")
else()
	set(OPENSCAP_VERSION "${OPENSCAP_VERSION_MAJOR}.${OPENSCAP_VERSION_MINOR}.${OPENSCAP_VERSION_PATCH}")
endif()

# libtool versioning
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details

## increment if the interface has additions, changes, removals.
set(LT_CURRENT 32)

## increment any time the source changes; set 0 to if you increment CURRENT
set(LT_REVISION 1)

## increment if any interfaces have been added; set to 0
## if any interfaces have been changed or removed. removal has
## precedence over adding, so set to 0 if both happened.
set(LT_AGE 7)

math(EXPR LT_CURRENT_MINUS_AGE "${LT_CURRENT} - ${LT_AGE}")

set(SONAME ${LT_CURRENT_MINUS_AGE}.${LT_AGE}.${LT_REVISION})
set(SOVERSION ${LT_CURRENT_MINUS_AGE})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

message(STATUS "OpenSCAP ${OPENSCAP_VERSION}")
message(STATUS "(see ${CMAKE_SOURCE_DIR}/docs/developer/developer.adoc for build instructions)")
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
include(CheckIncludeFiles)
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
find_package(DBUS)
find_package(Doxygen)
find_package(GConf)
find_package(Ldap)
find_package(OpenDbx)
if(WITH_PCRE2)
	find_package(PCRE2 REQUIRED)
else()
	find_package(PCRE REQUIRED)
endif()
find_package(PerlLibs)
find_package(Popt)
find_package(Systemd)

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

set(PREFERRED_PYTHON_PATH "${PYTHON_EXECUTABLE}" CACHE PATH "Path to preferred Python")
set(PYTHON3_PATH "${PYTHON_EXECUTABLE}" CACHE PATH "Path to Python3")

find_package(RPM)
if(RPM_FOUND)
	check_library_exists("${RPM_LIBRARY}" rpmReadConfigFiles "" HAVE_RPMREADCONFIGFILES)
	check_library_exists("${RPM_LIBRARY}" headerFormat "" HAVE_HEADERFORMAT)
	check_library_exists("${RPMIO_LIBRARY}" rpmFreeCrypto "" HAVE_RPMFREECRYPTO)
	check_library_exists("${RPM_LIBRARY}" rpmFreeFilesystems "" HAVE_RPMFREEFILESYSTEMS)
	check_library_exists("${RPM_LIBRARY}" rpmVerifyFile "" HAVE_RPMVERIFYFILE)
	set(HAVE_RPMVERCMP 1)
endif()

find_package(SELinux)
find_package(SWIG)
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
find_package(XMLSEC REQUIRED)
if (APPLE)
	set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl/")
endif()
find_package(OpenSSL REQUIRED)
add_definitions(${XMLSEC_DEFINITIONS})
if (WIN32)
	add_compile_definitions("XMLSEC_CRYPTO_OPENSSL")
endif()
find_package(BZip2)

# PThread
if (WIN32)
	find_package(ZLIB REQUIRED)
endif()

if (WIN32 AND NOT MINGW)
	find_package(pthread CONFIG REQUIRED)
	set(CMAKE_THREAD_LIBS_INIT ${PThreads4W_LIBRARY})
else()
	find_package(Threads REQUIRED)
endif()
set(CMAKE_THREAD_PREFER_PTHREAD)
set(THREADS_PREFER_PTHREAD_FLAG)
set(THREADS_USE_PTHREADS_WIN32 TRUE)
check_library_exists(pthread pthread_timedjoin_np "" HAVE_PTHREAD_TIMEDJOIN_NP)
check_library_exists(pthread pthread_setname_np "" HAVE_PTHREAD_SETNAME_NP)
check_library_exists(pthread pthread_getname_np "" HAVE_PTHREAD_GETNAME_NP)

# WITH_CRYPTO
set(WITH_CRYPTO "gcrypt" CACHE STRING "gcrypt|nss")
if(${WITH_CRYPTO} STREQUAL "nss")
	message("-- Using NSS")
	find_package(NSS)
else()
	message("-- Using GCrypt")
	find_package(GCrypt)
endif()
if(GCRYPT_FOUND OR NSS_FOUND)
	set(CRYPTO_FOUND TRUE)
endif()

find_package(Libyaml)
if(EXISTS ${CMAKE_SOURCE_DIR}/yaml-filter/CMakeLists.txt)
	message(STATUS "yaml-filter was found")
	set(YAML_FILTER_FOUND TRUE)
else()
	message(STATUS "yaml-filter was not found")
	set(YAML_FILTER_FOUND FALSE)
endif()

if(PCRE2_FOUND)
	set(HAVE_PCRE2 1)
	message("-- Using PCRE2")
elseif(PCRE_FOUND)
	message("-- Using PCRE")
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
check_include_file(sys/mman.h HAVE_MMAN_H)
check_include_file(sys/uio.h HAVE_UIO_H)
check_include_file(sys/xattr.h HAVE_SYS_XATTR_H)
check_include_file(attr/xattr.h HAVE_ATTR_XATTR_H)
check_include_files("sys/types.h;sys/extattr.h" HAVE_SYS_EXTATTR_H)

# HAVE_ATOMIC_BUILTINS
check_c_source_compiles("#include <stdint.h>\nint main() {uint16_t foovar=0; uint16_t old=1; uint16_t new=2;__sync_bool_compare_and_swap(&foovar,old,new); return __sync_fetch_and_add(&foovar, 1); __sync_fetch_and_add(&foovar, 1);}" HAVE_ATOMIC_BUILTINS)
if(NOT HAVE_ATOMIC_BUILTINS)
	message(WARNING "!!! Compiler does not support atomic builtins. Atomic operation will be emulated using mutex-based locking. !!!")
endif()

mark_as_advanced(ENV_PRESENT VALGRIND_PRESENT)
find_program(ENV_PRESENT env)
find_program(VALGRIND_PRESENT valgrind)
find_program(ASCIIDOC_EXECUTABLE asciidoc)
find_program(SED_EXECUTABLE sed)
find_program(GIT_EXECUTABLE git)

# ---------- CORE FEATURE SWITCHES
if(WIN32 OR APPLE)
	option(ENABLE_SCE "enables Script Check Engine - an alternative checking engine that lets you use executables instead of OVAL for checks" OFF)
else()
	option(ENABLE_SCE "enables Script Check Engine - an alternative checking engine that lets you use executables instead of OVAL for checks" ON)
endif()

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

option(OPENSCAP_ENABLE_SHA1 "Enable using the SHA-1 algorithm" ON)
option(OPENSCAP_ENABLE_MD5 "Enable using the MD5 algorithm" ON)

# INDEPENDENT PROBES
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE "Independent environmentvariable probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE58 "Independent environmentvariable58 probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_FAMILY "Independent family probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_FILEHASH "Independent filehash probe" ON "ENABLE_PROBES_INDEPENDENT; CRYPTO_FOUND; OPENSCAP_ENABLE_SHA1; OPENSCAP_ENABLE_MD5; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_FILEHASH58 "Independent filehash58 probe" ON "ENABLE_PROBES_INDEPENDENT; CRYPTO_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_SQL "Independent sql probe" ON "ENABLE_PROBES_INDEPENDENT; OPENDBX_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_SQL57 "Independent sql57 probe" ON "ENABLE_PROBES_INDEPENDENT; OPENDBX_FOUND; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_SYSTEM_INFO "Independent system info probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT "Independent textfilecontent probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT54 "Independent textfilecontent54 probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_VARIABLE "Independent variable probe" ON "ENABLE_PROBES_INDEPENDENT" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_XMLFILECONTENT "Independent xmlfilecontent probe" ON "ENABLE_PROBES_INDEPENDENT; NOT WIN32" OFF)
cmake_dependent_option(OPENSCAP_PROBE_INDEPENDENT_YAMLFILECONTENT "Independent yamlfilecontent probe" ON "ENABLE_PROBES_INDEPENDENT; LIBYAML_FOUND; YAML_FILTER_FOUND; NOT WIN32" OFF)

# UNIX PROBES
cmake_dependent_option(OPENSCAP_PROBE_UNIX_DNSCACHE "Unix dnscache probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_FILE "Unix file probe" ON "ENABLE_PROBES_UNIX" OFF)
cmake_dependent_option(OPENSCAP_PROBE_UNIX_FILEEXTENDEDATTRIBUTE "Unix fileextendedattribute probe" ON "ENABLE_PROBES_UNIX; HAVE_SYS_XATTR_H OR HAVE_ATTR_XATTR_H OR HAVE_SYS_EXTATTR_H" OFF)
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
cmake_dependent_option(OPENSCAP_PROBE_LINUX_DPKGINFO "Linux dpkginfo probe" ON "ENABLE_PROBES_LINUX" OFF)
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
cmake_dependent_option(OPENSCAP_PROBE_LINUX_FWUPDSECURITYATTR "Linux fwupd probe" ON "ENABLE_PROBES_LINUX; DBUS_FOUND" OFF)

# SOLARIS PROBES
cmake_dependent_option(OPENSCAP_PROBE_SOLARIS_ISAINFO "Solaris isainfo probe" ON "ENABLE_PROBES_SOLARIS" OFF)

# WINDOWS PROBES
cmake_dependent_option(OPENSCAP_PROBE_WINDOWS_ACCESSTOKEN "Windows accesstoken probe" ON "ENABLE_PROBES_WINDOWS" OFF)
cmake_dependent_option(OPENSCAP_PROBE_WINDOWS_REGISTRY "Windows registry probe" ON "ENABLE_PROBES_WINDOWS" OFF)
cmake_dependent_option(OPENSCAP_PROBE_WINDOWS_WMI57 "Windows wmi57 probe" ON "ENABLE_PROBES_WINDOWS" OFF)


# ---------- EXECUTABLES / UTILITIES SWITCHES

option(ENABLE_OSCAP_UTIL "build the oscap utility, oscap is the core utility for evaluation and processing of SCAP data" TRUE)

set(OSCAP_DOCKER_PYTHON ${PYTHON_EXECUTABLE} CACHE STRING "path to the Python interpreter for oscap-docker")
cmake_dependent_option(ENABLE_OSCAP_UTIL_DOCKER "enables the oscap-docker utility, this lets you scan containers and container images" ON "NOT WIN32" OFF)
if(ENABLE_OSCAP_UTIL_DOCKER AND NOT BZIP2_FOUND)
	set(ENABLE_OSCAP_UTIL_DOCKER FALSE)
	message(SEND_ERROR "oscap-docker requires bzip2! Either disable oscap-docker or install bzip2")
endif()

cmake_dependent_option(ENABLE_OSCAP_UTIL_AS_RPM "enable the scap-as-rpm utility, this lets you package SCAP data as RPMs" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_SSH "enables the oscap-ssh utility, this lets you scan remote machines over ssh" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_VM "enables the oscap-vm utility, this lets you scan VMs and VM storage images" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_PODMAN "enables the oscap-podman utility, this lets you scan Podman containers and container images" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_IM "enables the oscap-im utility, this lets you build hardened bootable container images" ON "NOT WIN32" OFF)
cmake_dependent_option(ENABLE_OSCAP_UTIL_CHROOT "enables the oscap-chroot utility, this lets you scan entire chroots using offline scanning" ON "NOT WIN32" OFF)
option(ENABLE_OSCAP_UTIL_AUTOTAILOR "enables the autotailor utility that is able to perform command-line tailoring" TRUE)
option(ENABLE_OSCAP_REMEDIATE_SERVICE "enables the oscap-remediate service" FALSE)

option(WITH_PCRE2 "use PCRE2 library" FALSE)

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

option(ENABLE_MITRE "enables MITRE tests -- requires specific environment support -- see developer documentation for more details" FALSE)

# ---------- LANGUAGE BINDINGS
cmake_dependent_option(ENABLE_PYTHON3 "if enabled, the python3 swig bindings will be built" ON "PYTHONINTERP_FOUND;SWIG_FOUND;PYTHONLIBS_FOUND" OFF)
cmake_dependent_option(ENABLE_PERL "if enabled, the perl swig bindings will be built" ON "PERLLIBS_FOUND;SWIG_FOUND;NOT WIN32" OFF)

# ---------- NO IDEA WHAT THIS IS FOR
set(WANT_BASE64 TRUE CACHE BOOL "wants builtin Base64")
set(WANT_XBASE64 FALSE CACHE BOOL "wants builtin XBase64")

# ---------- Documentation

# Due to the time it takes to build documentation on every change,
# we choose to disable documentation by default. Only when ENABLE_DOCS==TRUE
# will docs be built and added to the `make install` target.

option(ENABLE_DOCS "enables documentation building -- suggests doxygen, asciidoc" FALSE)

# ---------- STATUS MESSAGES

message(STATUS " ")
message(STATUS "CMake:")
message(STATUS "generator: ${CMAKE_GENERATOR}")
message(STATUS "source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "build directory: ${CMAKE_BINARY_DIR}")
message(STATUS " ")

message(STATUS "Core features:")
message(STATUS "SCE: ${ENABLE_SCE}")
message(STATUS " ")

message(STATUS "OVAL:")
message(STATUS "base probe support: ${ENABLE_PROBES}")
message(STATUS "SEAP msgid bit-size: ${SEAP_MSGID_BITS}")
message(STATUS "SHA-1: ${OPENSCAP_ENABLE_SHA1}")
message(STATUS "MD5: ${OPENSCAP_ENABLE_MD5}")

message(STATUS "")
message(STATUS "Independent probes: ${ENABLE_PROBES_INDEPENDENT}")
message(STATUS "  Independent environmentvariable probe: ${OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE}")
message(STATUS "  Independent family probe: ${OPENSCAP_PROBE_INDEPENDENT_FAMILY}")
message(STATUS "  Independent system info probe: ${OPENSCAP_PROBE_INDEPENDENT_SYSTEM_INFO}")
message(STATUS "  Independent variable probe: ${OPENSCAP_PROBE_INDEPENDENT_VARIABLE}")

message(STATUS "")
message(STATUS "Independent probes incompatible with WIN32 (WIN32 status: ${IS_WIN32})")
message(STATUS "  Independent environmentvariable58 probe: ${OPENSCAP_PROBE_INDEPENDENT_ENVIRONMENTVARIABLE58}")
message(STATUS "  Independent filehash probe: ${OPENSCAP_PROBE_INDEPENDENT_FILEHASH}")
message(STATUS "  Independent filehash58 probe: ${OPENSCAP_PROBE_INDEPENDENT_FILEHASH58}")
message(STATUS "  Independent sql probe (depends on opendbx): ${OPENSCAP_PROBE_INDEPENDENT_SQL}")
message(STATUS "  Independent sql57 probe (depends on opendbx): ${OPENSCAP_PROBE_INDEPENDENT_SQL57}")
message(STATUS "  Independent textfilecontent probe: ${OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT}")
message(STATUS "  Independent textfilecontent54 probe: ${OPENSCAP_PROBE_INDEPENDENT_TEXTFILECONTENT54}")
message(STATUS "  Independent xmlfilecontent probe: ${OPENSCAP_PROBE_INDEPENDENT_XMLFILECONTENT}")
message(STATUS "  Independent yamlfilecontent probe (depends on libyaml, yaml-path): ${OPENSCAP_PROBE_INDEPENDENT_YAMLFILECONTENT}")
message(STATUS " ")


message(STATUS "Unix probes: ${ENABLE_PROBES_UNIX}")
message(STATUS "  Unix dnscache probe: ${OPENSCAP_PROBE_UNIX_DNSCACHE}")
message(STATUS "  Unix file probe: ${OPENSCAP_PROBE_UNIX_FILE}")
message(STATUS "  Unix fileextendedattribute probe (depends on xattrh): ${OPENSCAP_PROBE_UNIX_FILEEXTENDEDATTRIBUTE}")
message(STATUS "  Unix gconf probe (depends on gconf): ${OPENSCAP_PROBE_UNIX_GCONF}")
message(STATUS "  Unix interface probe: ${OPENSCAP_PROBE_UNIX_INTERFACE}")
message(STATUS "  Unix password probe: ${OPENSCAP_PROBE_UNIX_PASSWORD}")
message(STATUS "  Unix process probe: ${OPENSCAP_PROBE_UNIX_PROCESS}")
message(STATUS "  Unix process58 probe (depends on CAP): ${OPENSCAP_PROBE_UNIX_PROCESS58}")
message(STATUS "  Unix routingtable probe: ${OPENSCAP_PROBE_UNIX_ROUTINGTABLE}")
message(STATUS "  Unix runlevel probe: ${OPENSCAP_PROBE_UNIX_RUNLEVEL}")
message(STATUS "  Unix shadow probe: ${OPENSCAP_PROBE_UNIX_SHADOW}")
message(STATUS "  Unix symlink probe: ${OPENSCAP_PROBE_UNIX_SYMLINK}")
message(STATUS "  Unix sysctl probe: ${OPENSCAP_PROBE_UNIX_SYSCTL}")
message(STATUS "  Unix uname probe: ${OPENSCAP_PROBE_UNIX_UNAME}")
message(STATUS "  Unix xinetd probe: ${OPENSCAP_PROBE_UNIX_XINETD}")
message(STATUS " ")

message(STATUS "Linux probes: ${ENABLE_PROBES_LINUX}")
message(STATUS "  Linux dpkginfo probe: ${OPENSCAP_PROBE_LINUX_DPKGINFO}")
message(STATUS "  Linux iflisteners probe: ${OPENSCAP_PROBE_LINUX_IFLISTENERS}")
message(STATUS "  Linux inetlisteningservers probe: ${OPENSCAP_PROBE_LINUX_INETLISTENINGSERVERS}")
message(STATUS "  Linux partition probe (depends on blkid): ${OPENSCAP_PROBE_LINUX_PARTITION}")
message(STATUS "  Linux rpminfo probe (depends on rpm): ${OPENSCAP_PROBE_LINUX_RPMINFO}")
message(STATUS "  Linux rpmverify probe (depends on rpm): ${OPENSCAP_PROBE_LINUX_RPMVERIFY}")
message(STATUS "  Linux rpmverifyfile probe (depends on rpm): ${OPENSCAP_PROBE_LINUX_RPMVERIFYFILE}")
message(STATUS "  Linux rpmverifypackage probe (depends on rpm): ${OPENSCAP_PROBE_LINUX_RPMVERIFYPACKAGE}")
message(STATUS "  Linux selinuxboolean probe (depends on selinux): ${OPENSCAP_PROBE_LINUX_SELINUXBOOLEAN}")
message(STATUS "  Linux selinuxsecuritycontext probe (depends on selinux): ${OPENSCAP_PROBE_LINUX_SELINUXSECURITYCONTEXT}")
message(STATUS "  Linux systemdunitdependency probe (depends on dbus): ${OPENSCAP_PROBE_LINUX_SYSTEMDUNITDEPENDENCY}")
message(STATUS "  Linux systemdunitproperty probe (depends on dbus): ${OPENSCAP_PROBE_LINUX_SYSTEMDUNITPROPERTY}")
message(STATUS "  Linux fwupdsecurityattr probe (depends on dbus): ${OPENSCAP_PROBE_LINUX_FWUPDSECURITYATTR}")
message(STATUS " ")

message(STATUS "Solaris probes: ${ENABLE_PROBES_SOLARIS}")
message(STATUS "  Solaris isainfo probe: ${OPENSCAP_PROBE_SOLARIS_ISAINFO}")
message(STATUS " ")


message(STATUS "Windows probes: ${ENABLE_PROBES_WINDOWS}")
message(STATUS "  Windows accesstoken probe: ${OPENSCAP_PROBE_WINDOWS_ACCESSTOKEN}")
message(STATUS "  Windows registry probe: ${OPENSCAP_PROBE_WINDOWS_REGISTRY}")
message(STATUS "  Windows wmi57 probe: ${OPENSCAP_PROBE_WINDOWS_WMI57}")
message(STATUS " ")


message(STATUS "Language bindings:")
message(STATUS "python3 bindings: ${ENABLE_PYTHON3}")
message(STATUS "perl bindings: ${ENABLE_PERL}")
message(STATUS "perl vendorlib: ${PERL_VENDORLIB}")
message(STATUS "perl vendorarch: ${PERL_VENDORARCH}")
message(STATUS "perl privlib: ${PERL_PRIVLIB}")
message(STATUS "perl archlib: ${PERL_ARCHLIB}")
message(STATUS " ")

message(STATUS "Utilities:")
message(STATUS "oscap: ${ENABLE_OSCAP_UTIL}")
message(STATUS "oscap-docker: ${ENABLE_OSCAP_UTIL_DOCKER}")
message(STATUS "scap-as-rpm: ${ENABLE_OSCAP_UTIL_AS_RPM}")
message(STATUS "oscap-ssh: ${ENABLE_OSCAP_UTIL_SSH}")
message(STATUS "oscap-vm: ${ENABLE_OSCAP_UTIL_VM}")
message(STATUS "oscap-podman: ${ENABLE_OSCAP_UTIL_PODMAN}")
message(STATUS "oscap-im: ${ENABLE_OSCAP_UTIL_IM}")
message(STATUS "oscap-chroot: ${ENABLE_OSCAP_UTIL_CHROOT}")
message(STATUS "autotailor: ${ENABLE_OSCAP_UTIL_AUTOTAILOR}")
message(STATUS " ")

message(STATUS "Testing:")
message(STATUS "tests: ${ENABLE_TESTS}")
message(STATUS "valgrind: ${ENABLE_VALGRIND}")
message(STATUS "MITRE: ${ENABLE_MITRE}")
message(STATUS " ")

message(STATUS "Documentation:")
message(STATUS "enabled: ${ENABLE_DOCS}")
message(STATUS "doxygen: ${DOXYGEN_EXECUTABLE}")
message(STATUS "dot (graphviz): ${DOXYGEN_DOT_EXECUTABLE}")
message(STATUS "asciidoc: ${ASCIIDOC_EXECUTABLE}")

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
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -W -Wall -Wnonnull -Wshadow -Wformat -Wundef -Wno-unused-parameter -Wmissing-prototypes -Wno-unknown-pragmas -Wno-int-conversion -Werror=implicit-function-declaration -D_GNU_SOURCE -std=c99")
	add_link_options(-Wl,-z,now)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	add_link_options(-lkvm -lm -lprocstat)
else()
	# We do not define this on FreeBSD as it causes required functionality to not be exposed, e.g. getprogname() and u_<types>
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200112L")
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
	"yaml-filter/src/"
	${CMAKE_BINARY_DIR} # config.h is generated to build directory
	${LIBXML2_INCLUDE_DIR}
	${XMLSEC_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	${PCRE_INCLUDE_DIRS}
	${PCRE2_INCLUDE_DIRS}
)

# Honor visibility properties for all target types
# Run "cmake --help-policy CMP0063" for policy details
if (POLICY CMP0063)
	cmake_policy(SET CMP0063 NEW)
else()
	message(WARNING "It is not possible to correctly set symbol visibility in object files with your version of CMake. We recommend using CMake 3.3 or newer.")
endif()

function(set_oscap_generic_properties TARGET_OBJECT)
	set_target_properties(${TARGET_OBJECT} PROPERTIES
		# Make global variables and functions HIDDEN by default.
		C_VISIBILITY_PRESET hidden
		CXX_VISIBILITY_PRESET hidden
		POSITION_INDEPENDENT_CODE ON    # Compile this object code position independent.
	)
	target_compile_definitions(${TARGET_OBJECT} PRIVATE OSCAP_BUILD_SHARED)
endfunction()

if(OPENSCAP_PROBE_INDEPENDENT_YAMLFILECONTENT)
	add_library(yamlfilter_object OBJECT yaml-filter/src/yaml-path.c yaml-filter/src/yaml-path.h)
	set_oscap_generic_properties(yamlfilter_object)
endif()

add_subdirectory("compat")
add_subdirectory("src")
add_subdirectory("utils")
add_subdirectory("docs")
add_subdirectory("dist")
add_subdirectory("schemas")
add_subdirectory("xsl")
add_subdirectory("cpe")
add_subdirectory("swig")
configure_file("run.in" ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/run @ONLY)
configure_file("oscap_wrapper.in" ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/oscap_wrapper @ONLY)
file(
	COPY "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/run" "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/oscap_wrapper"
	DESTINATION ${CMAKE_BINARY_DIR}
	FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

if(NOT WIN32)
	# pkgconfig file
	configure_file("libopenscap.pc.in" "libopenscap.pc" @ONLY)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/libopenscap.pc
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	)
	if(WITH_SYSTEMD)
		if(ENABLE_OSCAP_REMEDIATE_SERVICE)
			# systemd service for offline (boot-time) remediation
			configure_file("oscap-remediate.service.in" "oscap-remediate.service" @ONLY)
			install(FILES
				${CMAKE_CURRENT_BINARY_DIR}/oscap-remediate.service
				DESTINATION ${CMAKE_INSTALL_PREFIX}/${SYSTEMD_UNITDIR}
			)
		endif()
	endif()
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
if(WIN32)
	set(CPACK_GENERATOR WIX)
	set(CPACK_WIX_PATCH_FILE "${CMAKE_SOURCE_DIR}/wix_patch.xml")
endif()
set(CPACK_PACKAGE_NAME "OpenSCAP")
set(CPACK_PACKAGE_VENDOR "OpenSCAP Project")
set(CPACK_PACKAGE_VERSION_MAJOR "${OPENSCAP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${OPENSCAP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${OPENSCAP_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/lgpl-2.1.rtf")
set(CPACK_PACKAGE_CHECKSUM SHA512)

include(CPack)
