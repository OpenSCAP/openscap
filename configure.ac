# ! MAKE SURE YOU ARE EDITING THE ac_probes/configure.ac.tpl FILE,
# ! THE configure.ac FILE ITSELF IS GENERATED FROM THE TEMPLATE USING
# ! ac_probes/ac_probes.sh

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([openscap], [1.1.2], [open-scap-list@redhat.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign tar-pax])

# If automake supports "silent rules", enable them by default
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_DISABLE_STATIC
#build dll on windows(cygwin)
AC_LIBTOOL_WIN32_DLL
AM_PATH_PYTHON

# Checks for programs.
AC_PROG_CC
gl_EARLY
gl_INIT
AM_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# swig
AC_PROG_SWIG([])

# libtool versioning
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details

## increment if the interface has additions, changes, removals.
LT_CURRENT=11

## increment any time the source changes; set 0 to if you increment CURRENT
LT_REVISION=2

## increment if any interfaces have been added; set to 0
## if any interfaces have been changed or removed. removal has
## precedence over adding, so set to 0 if both happened.
LT_AGE=3

LT_CURRENT_MINUS_AGE=`expr $LT_CURRENT - $LT_AGE`

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LT_CURRENT_MINUS_AGE)

AC_DEFINE_UNQUOTED([LT_CURRENT_MINUS_AGE], [$LT_CURRENT_MINUS_AGE], [LT_CURRENT - LT_AGE])

AC_DEFUN([canonical_wrap], [AC_REQUIRE([AC_CANONICAL_HOST])])
canonical_wrap

# Compiler flags
CFLAGS="$CFLAGS -pipe -std=c99 -W -Wall -Wnonnull -Wshadow -Wformat -Wundef -Wno-unused-parameter -Wmissing-prototypes -Wno-unknown-pragmas -D_GNU_SOURCE -DOSCAP_THREAD_SAFE -D_POSIX_C_SOURCE=200112L"

case $host in
  *solaris*) :
    CFLAGS="$CFLAGS -D__EXTENSIONS__" ;;
esac

CFLAGS_OPTIMIZED="-O2 -finline-functions"
CFLAGS_DEBUGGING="-fno-inline-functions -O0 -g3"
CFLAGS_NODEBUG="-Wno-unused-function"

my_save_cflags="$CFLAGS"
CFLAGS="$CFLAGS -Werror=format-security"
AC_MSG_CHECKING([whether CC supports -Werror=format-security])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [AC_MSG_RESULT([yes])]
    [AM_CFLAGS="-Werror=format-security"],
    [AC_MSG_RESULT([no])]
)
CFLAGS="$my_save_cflags"
AC_SUBST([AM_CFLAGS])


probe_system_info_req_deps_ok=yes
probe_system_info_req_deps_missing=
probe_system_info_opt_deps_ok=yes
probe_system_info_opt_deps_missing=
probe_family_req_deps_ok=yes
probe_family_req_deps_missing=
probe_family_opt_deps_ok=yes
probe_family_opt_deps_missing=
probe_textfilecontent_req_deps_ok=yes
probe_textfilecontent_req_deps_missing=
probe_textfilecontent_opt_deps_ok=yes
probe_textfilecontent_opt_deps_missing=
probe_textfilecontent54_req_deps_ok=yes
probe_textfilecontent54_req_deps_missing=
probe_textfilecontent54_opt_deps_ok=yes
probe_textfilecontent54_opt_deps_missing=
probe_variable_req_deps_ok=yes
probe_variable_req_deps_missing=
probe_variable_opt_deps_ok=yes
probe_variable_opt_deps_missing=
probe_xmlfilecontent_req_deps_ok=yes
probe_xmlfilecontent_req_deps_missing=
probe_xmlfilecontent_opt_deps_ok=yes
probe_xmlfilecontent_opt_deps_missing=
probe_filehash_req_deps_ok=yes
probe_filehash_req_deps_missing=
probe_filehash_opt_deps_ok=yes
probe_filehash_opt_deps_missing=
probe_filehash58_req_deps_ok=yes
probe_filehash58_req_deps_missing=
probe_filehash58_opt_deps_ok=yes
probe_filehash58_opt_deps_missing=
probe_environmentvariable_req_deps_ok=yes
probe_environmentvariable_req_deps_missing=
probe_environmentvariable_opt_deps_ok=yes
probe_environmentvariable_opt_deps_missing=
probe_environmentvariable58_req_deps_ok=yes
probe_environmentvariable58_req_deps_missing=
probe_environmentvariable58_opt_deps_ok=yes
probe_environmentvariable58_opt_deps_missing=
probe_sql_req_deps_ok=yes
probe_sql_req_deps_missing=
probe_sql_opt_deps_ok=yes
probe_sql_opt_deps_missing=
probe_sql57_req_deps_ok=yes
probe_sql57_req_deps_missing=
probe_sql57_opt_deps_ok=yes
probe_sql57_opt_deps_missing=
probe_ldap57_req_deps_ok=yes
probe_ldap57_req_deps_missing=
probe_ldap57_opt_deps_ok=yes
probe_ldap57_opt_deps_missing=
probe_dnscache_req_deps_ok=yes
probe_dnscache_req_deps_missing=
probe_dnscache_opt_deps_ok=yes
probe_dnscache_opt_deps_missing=
probe_runlevel_req_deps_ok=yes
probe_runlevel_req_deps_missing=
probe_runlevel_opt_deps_ok=yes
probe_runlevel_opt_deps_missing=
probe_file_req_deps_ok=yes
probe_file_req_deps_missing=
probe_file_opt_deps_ok=yes
probe_file_opt_deps_missing=
probe_fileextendedattribute_req_deps_ok=yes
probe_fileextendedattribute_req_deps_missing=
probe_fileextendedattribute_opt_deps_ok=yes
probe_fileextendedattribute_opt_deps_missing=
probe_password_req_deps_ok=yes
probe_password_req_deps_missing=
probe_password_opt_deps_ok=yes
probe_password_opt_deps_missing=
probe_process_req_deps_ok=yes
probe_process_req_deps_missing=
probe_process_opt_deps_ok=yes
probe_process_opt_deps_missing=
probe_process58_req_deps_ok=yes
probe_process58_req_deps_missing=
probe_process58_opt_deps_ok=yes
probe_process58_opt_deps_missing=
probe_shadow_req_deps_ok=yes
probe_shadow_req_deps_missing=
probe_shadow_opt_deps_ok=yes
probe_shadow_opt_deps_missing=
probe_uname_req_deps_ok=yes
probe_uname_req_deps_missing=
probe_uname_opt_deps_ok=yes
probe_uname_opt_deps_missing=
probe_interface_req_deps_ok=yes
probe_interface_req_deps_missing=
probe_interface_opt_deps_ok=yes
probe_interface_opt_deps_missing=
probe_xinetd_req_deps_ok=yes
probe_xinetd_req_deps_missing=
probe_xinetd_opt_deps_ok=yes
probe_xinetd_opt_deps_missing=
probe_sysctl_req_deps_ok=yes
probe_sysctl_req_deps_missing=
probe_sysctl_opt_deps_ok=yes
probe_sysctl_opt_deps_missing=
probe_routingtable_req_deps_ok=yes
probe_routingtable_req_deps_missing=
probe_routingtable_opt_deps_ok=yes
probe_routingtable_opt_deps_missing=
probe_gconf_req_deps_ok=yes
probe_gconf_req_deps_missing=
probe_gconf_opt_deps_ok=yes
probe_gconf_opt_deps_missing=
probe_isainfo_req_deps_ok=yes
probe_isainfo_req_deps_missing=
probe_isainfo_opt_deps_ok=yes
probe_isainfo_opt_deps_missing=
probe_package_req_deps_ok=yes
probe_package_req_deps_missing=
probe_package_opt_deps_ok=yes
probe_package_opt_deps_missing=
probe_patch_req_deps_ok=yes
probe_patch_req_deps_missing=
probe_patch_opt_deps_ok=yes
probe_patch_opt_deps_missing=
probe_smf_req_deps_ok=yes
probe_smf_req_deps_missing=
probe_smf_opt_deps_ok=yes
probe_smf_opt_deps_missing=
probe_partition_req_deps_ok=yes
probe_partition_req_deps_missing=
probe_partition_opt_deps_ok=yes
probe_partition_opt_deps_missing=
probe_inetlisteningservers_req_deps_ok=yes
probe_inetlisteningservers_req_deps_missing=
probe_inetlisteningservers_opt_deps_ok=yes
probe_inetlisteningservers_opt_deps_missing=
probe_iflisteners_req_deps_ok=yes
probe_iflisteners_req_deps_missing=
probe_iflisteners_opt_deps_ok=yes
probe_iflisteners_opt_deps_missing=
probe_selinuxboolean_req_deps_ok=yes
probe_selinuxboolean_req_deps_missing=
probe_selinuxboolean_opt_deps_ok=yes
probe_selinuxboolean_opt_deps_missing=
probe_selinuxsecuritycontext_req_deps_ok=yes
probe_selinuxsecuritycontext_req_deps_missing=
probe_selinuxsecuritycontext_opt_deps_ok=yes
probe_selinuxsecuritycontext_opt_deps_missing=
probe_rpminfo_req_deps_ok=yes
probe_rpminfo_req_deps_missing=
probe_rpminfo_opt_deps_ok=yes
probe_rpminfo_opt_deps_missing=
probe_rpmverify_req_deps_ok=yes
probe_rpmverify_req_deps_missing=
probe_rpmverify_opt_deps_ok=yes
probe_rpmverify_opt_deps_missing=
probe_rpmverifyfile_req_deps_ok=yes
probe_rpmverifyfile_req_deps_missing=
probe_rpmverifyfile_opt_deps_ok=yes
probe_rpmverifyfile_opt_deps_missing=
probe_rpmverifypackage_req_deps_ok=yes
probe_rpmverifypackage_req_deps_missing=
probe_rpmverifypackage_opt_deps_ok=yes
probe_rpmverifypackage_opt_deps_missing=
probe_dpkginfo_req_deps_ok=yes
probe_dpkginfo_req_deps_missing=
probe_dpkginfo_opt_deps_ok=yes
probe_dpkginfo_opt_deps_missing=
probe_systemdunitproperty_req_deps_ok=yes
probe_systemdunitproperty_req_deps_missing=
probe_systemdunitproperty_opt_deps_ok=yes
probe_systemdunitproperty_opt_deps_missing=
probe_systemdunitdependency_req_deps_ok=yes
probe_systemdunitdependency_req_deps_missing=
probe_systemdunitdependency_opt_deps_ok=yes
probe_systemdunitdependency_opt_deps_missing=

#
# env
#
AC_CHECK_PROG(
  [HAVE_ENV],
  [env],
  [yes],,,
)

AM_CONDITIONAL(ENV_PRESENT, [test x"${HAVE_ENV}" = xyes])

#
# Valgrind
#
AC_CHECK_PROG(
  [HAVE_VALGRIND],
  [valgrind],
  [yes],,,
)

AM_CONDITIONAL(VALGRIND_PRESENT, [test x"${HAVE_VALGRIND}" = xyes])

AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_FUNC_REALLOC

AM_CHECK_PYTHON_HEADERS

#
# threads
#
pthread_CFLAGS=error
pthread_LIBS=error
SAVE_LIBS=$LIBS

if test "x$pthread_LIBS" = "xerror"; then
   AC_CHECK_LIB(c_r, pthread_attr_init, [
                     pthread_CFLAGS="-DOSCAP_THREAD_SAFE -D_THREAD_SAFE -pthread"
                     pthread_LIBS="-pthread -lrt" ])
fi

if test "x$pthread_LIBS" = "xerror"; then
   AC_CHECK_LIB(pthread, pthread_attr_init, [
                         pthread_CFLAGS="-DOSCAP_THREAD_SAFE -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS"
                         pthread_LIBS="-lpthread -lrt" ])
fi

if test "x$pthread_LIBS" = "xerror"; then
   AC_CHECK_LIB(pthreads, pthread_attr_init, [
                          pthread_CFLAGS="-DOSCAP_THREAD_SAFE -D_THREAD_SAFE"
                          pthread_LIBS="-lpthreads -lrt" ])
fi

if test "x$pthread_LIBS" = "xerror"; then
   AC_MSG_FAILURE(pthread library is missing)
fi

SAVE_CFLAGS=$CFLAGS
CFLAGS="$CFLAGS -D_GNU_SOURCE"
LIBS="$pthread_LIBS"
AC_CHECK_FUNCS([pthread_timedjoin_np clock_gettime])
CFLAGS=$SAVE_CFLAGS

LIBS=$SAVE_LIBS
AC_SUBST(pthread_CFLAGS)
AC_SUBST(pthread_LIBS)

PKG_CHECK_MODULES([curl], [libcurl >= 7.12.0],[],
                          AC_MSG_FAILURE([libcurl devel support is missing]))

PKG_CHECK_MODULES([xml2], [libxml-2.0 >= 2.0],[],
			  AC_MSG_FAILURE([libxml-2.0 devel support is missing]))

PKG_CHECK_MODULES([xslt], [libxslt >= 1.1],[],
			  AC_MSG_FAILURE([libxslt devel support is missing]))

PKG_CHECK_MODULES([exslt], [libexslt >= 0.8],[],
			  AC_MSG_FAILURE([libexslt devel support is missing]))

AC_CHECK_HEADER(pcre.h, , [AC_MSG_ERROR([pcre.h is missing] )])

crapi_CFLAGS=""
crapi_LIBS=""

if test "${with_crypto}" = ""; then
   with_crypto=gcrypt
fi

case "${with_crypto}" in
      nss3)
	PKG_CHECK_MODULES([nss3], [nss >= 3.0],[],
			  AC_MSG_FAILURE([libnss3 devel support is missing]))

	crapi_libname="NSS 3.x"
	crapi_CFLAGS=$nss3_CFLAGS
	crapi_LIBS=$nss3_LIBS
        AC_DEFINE([HAVE_NSS3], [1], [Define to 1 if you have 'NSS' library.])
        ;;
    gcrypt)
	SAVE_LIBS=$LIBS
        AC_CHECK_LIB([gcrypt], [gcry_check_version],
                     [crapi_CFLAGS=`libgcrypt-config --cflags`;
                      crapi_LIBS=`libgcrypt-config --libs`;
                      crapi_libname="GCrypt";],
                     [AC_MSG_ERROR([library 'gcrypt' is required for GCrypt.])],
                     [])
        AC_DEFINE([HAVE_GCRYPT], [1], [Define to 1 if you have 'gcrypt' library.])
	AC_CACHE_CHECK([for GCRYCTL_SET_ENFORCED_FIPS_FLAG],
                    [ac_cv_gcryctl_set_enforced_fips_flag],
                    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include<gcrypt.h>],
                                                        [return GCRYCTL_SET_ENFORCED_FIPS_FLAG;])],
                                       [ac_cv_gcryctl_set_enforced_fips_flag=yes],
                                       [ac_cv_gcryctl_set_enforced_fips_flag=no])])

	if test "${ac_cv_gcryctl_set_enforced_fips_flag}" == "yes"; then
	   AC_DEFINE([HAVE_GCRYCTL_SET_ENFORCED_FIPS_FLAG], [1], [Define to 1 if you have 'gcrypt' library with GCRYCTL_SET_ENFORCED_FIPS_FLAG.])
	fi
	LIBS=$SAVE_LIBS
        ;;
         *)
          AC_MSG_ERROR([unknown crypto backend])
        ;;
esac

AC_SUBST(crapi_CFLAGS)
AC_SUBST(crapi_LIBS)

AC_CHECK_FUNCS([fts_open posix_memalign memalign])
AC_CHECK_FUNC(sigwaitinfo, [sigwaitinfo_LIBS=""], [sigwaitinfo_LIBS="-lrt"])
AC_SUBST(sigwaitinfo_LIBS)

# libopenscap links against librpm if found. Otherwise we carry own implementation of rpmvercmp.
echo
echo '* Checking for rpm library  (optional dependency of libopenscap) '
PKG_CHECK_MODULES([rpm], [rpm >= 4.4],[
	SAVE_LIBS=$LIBS
	AC_DEFINE([HAVE_RPMVERCMP], [1], [Define to 1 if there is rpmvercmp available.])
	AC_SUBST([rpm_CFLAGS])
	AC_SUBST([rpm_LIBS])
	LIBS=$SAVE_LIBS
],[
	AC_MSG_NOTICE([!!! librpm not found. The rpmvercmp function will be emulated. !!!])
])


SAVE_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS  $(pkg-config libapt-pkg --cflags) $(pkg-config blkid --cflags) $(pkg-config dbus-1 --cflags) $(pkg-config gconf-2.0 --cflags) $(pkg-config libpcre --cflags) $(pkg-config libprocps --cflags) $(pkg-config rpm --cflags) $(pkg-config libselinux --cflags) $(pkg-config libxml-2.0 --cflags) $(pkg-config libxslt --cflags) "

echo
echo ' * Checking presence of required headers for the system_info probe'
AC_CHECK_HEADERS([arpa/inet.h ctype.h errno.h ifaddrs.h libdlpi.h netdb.h net/if.h net/if_types.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/sockio.h sys/utsname.h unistd.h ],[],[probe_system_info_req_deps_ok=no; probe_system_info_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the family probe'
AC_CHECK_HEADERS([string.h ],[],[probe_family_req_deps_ok=no; probe_family_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the textfilecontent probe'
AC_CHECK_HEADERS([fcntl.h limits.h pcre.h regex.h stdio.h string.h sys/stat.h sys/types.h ],[],[probe_textfilecontent_req_deps_ok=no; probe_textfilecontent_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the textfilecontent54 probe'
AC_CHECK_HEADERS([errno.h fcntl.h limits.h pcre.h regex.h stdio.h stdlib.h string.h sys/stat.h sys/types.h unistd.h ],[],[probe_textfilecontent54_req_deps_ok=no; probe_textfilecontent54_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the variable probe'
AC_CHECK_HEADERS([errno.h stdio.h stdlib.h string.h ],[],[probe_variable_req_deps_ok=no; probe_variable_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the xmlfilecontent probe'
AC_CHECK_HEADERS([libxml/parser.h libxml/tree.h libxml/xpath.h libxml/xpathInternals.h limits.h stdlib.h string.h ],[],[probe_xmlfilecontent_req_deps_ok=no; probe_xmlfilecontent_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the filehash probe'
AC_CHECK_HEADERS([errno.h fcntl.h limits.h pthread.h stdlib.h string.h sys/stat.h sys/types.h ],[],[probe_filehash_req_deps_ok=no; probe_filehash_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the filehash58 probe'
AC_CHECK_HEADERS([errno.h fcntl.h limits.h pthread.h stdlib.h string.h sys/stat.h sys/types.h ],[],[probe_filehash58_req_deps_ok=no; probe_filehash58_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the environmentvariable probe'
AC_CHECK_HEADERS([errno.h stdio.h stdlib.h string.h ],[],[probe_environmentvariable_req_deps_ok=no; probe_environmentvariable_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the environmentvariable58 probe'
AC_CHECK_HEADERS([dirent.h errno.h fcntl.h stdio.h stdlib.h string.h sys/stat.h sys/types.h unistd.h ],[],[probe_environmentvariable58_req_deps_ok=no; probe_environmentvariable58_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the sql probe'
AC_CHECK_HEADERS([ctype.h errno.h opendbx/api.h stdlib.h string.h time.h ],[],[probe_sql_req_deps_ok=no; probe_sql_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the sql57 probe'
AC_CHECK_HEADERS([ctype.h errno.h opendbx/api.h stdlib.h string.h time.h ],[],[probe_sql57_req_deps_ok=no; probe_sql57_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the ldap57 probe'
AC_CHECK_HEADERS([ldap.h pthread.h stdbool.h ],[],[probe_ldap57_req_deps_ok=no; probe_ldap57_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the dnscache probe'
AC_CHECK_HEADERS([string.h ],[],[probe_dnscache_req_deps_ok=no; probe_dnscache_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the runlevel probe'
AC_CHECK_HEADERS([assert.h dirent.h errno.h limits.h stdbool.h stdio.h string.h sys/stat.h sys/types.h unistd.h ],[],[probe_runlevel_req_deps_ok=no; probe_runlevel_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the file probe'
AC_CHECK_HEADERS([errno.h limits.h pthread.h stdlib.h string.h sys/stat.h ],[],[probe_file_req_deps_ok=no; probe_file_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of optional headers for the file probe'
AC_CHECK_HEADERS([acl/libacl.h sys/acl.h sys/types.h ],[],[probe_file_opt_deps_ok=no],[-])

echo
echo ' * Checking presence of required headers for the fileextendedattribute probe'
AC_CHECK_HEADERS([attr/xattr.h errno.h limits.h pthread.h stdlib.h string.h sys/stat.h sys/types.h ],[],[probe_fileextendedattribute_req_deps_ok=no; probe_fileextendedattribute_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the password probe'
AC_CHECK_HEADERS([errno.h lastlog.h paths.h pwd.h stdio.h stdlib.h string.h ],[],[probe_password_req_deps_ok=no; probe_password_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the process probe'
AC_CHECK_HEADERS([dirent.h errno.h sched.h time.h ],[],[probe_process_req_deps_ok=no; probe_process_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of optional headers for the process probe'
AC_CHECK_HEADERS([fcntl.h proc/devname.h stdio_ext.h stdio.h stdlib.h string.h sys/stat.h sys/sysmacros.h sys/types.h unistd.h ],[],[probe_process_opt_deps_ok=no],[-])

echo
echo ' * Checking presence of required headers for the process58 probe'
AC_CHECK_HEADERS([dirent.h errno.h sched.h time.h ],[],[probe_process58_req_deps_ok=no; probe_process58_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of optional headers for the process58 probe'
AC_CHECK_HEADERS([ctype.h fcntl.h proc/devname.h selinux/context.h selinux/selinux.h stdio_ext.h stdio.h stdlib.h string.h sys/capability.h sys/stat.h sys/sysmacros.h sys/types.h unistd.h ],[],[probe_process58_opt_deps_ok=no],[-])

echo
echo ' * Checking presence of required headers for the shadow probe'
AC_CHECK_HEADERS([errno.h shadow.h stdio.h stdlib.h string.h ],[],[probe_shadow_req_deps_ok=no; probe_shadow_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the uname probe'
AC_CHECK_HEADERS([string.h sys/utsname.h ],[],[probe_uname_req_deps_ok=no; probe_uname_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of optional headers for the uname probe'
AC_CHECK_HEADERS([stdio_ext.h sys/systeminfo.h ],[],[probe_uname_opt_deps_ok=no],[-])

echo
echo ' * Checking presence of required headers for the interface probe'
AC_CHECK_HEADERS([arpa/inet.h ifaddrs.h netdb.h net/if_arp.h net/if.h stdio.h string.h sys/ioctl.h sys/socket.h unistd.h ],[],[probe_interface_req_deps_ok=no; probe_interface_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the xinetd probe'
AC_CHECK_HEADERS([ctype.h dirent.h errno.h fcntl.h fnmatch.h limits.h netdb.h stdbool.h stddef.h stdint.h stdlib.h string.h sys/stat.h sys/types.h unistd.h ],[],[probe_xinetd_req_deps_ok=no; probe_xinetd_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the sysctl probe'
AC_CHECK_HEADERS([ctype.h errno.h limits.h stdio.h string.h ],[],[probe_sysctl_req_deps_ok=no; probe_sysctl_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the routingtable probe'
AC_CHECK_HEADERS([arpa/inet.h byteswap.h endian.h errno.h netinet/in.h netinet/ip6.h netinet/ip.h net/route.h stdio.h stdlib.h string.h ],[],[probe_routingtable_req_deps_ok=no; probe_routingtable_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the gconf probe'
AC_CHECK_HEADERS([gconf/gconf.h limits.h pcre.h stdint.h stdlib.h string.h ],[],[probe_gconf_req_deps_ok=no; probe_gconf_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the isainfo probe'
AC_CHECK_HEADERS([arpa/inet.h dirent.h errno.h fcntl.h netdb.h regex.h stdio_ext.h stdio.h stdlib.h string.h sys/systeminfo.h ],[],[probe_isainfo_req_deps_ok=no; probe_isainfo_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the partition probe'
AC_CHECK_HEADERS([fcntl.h limits.h mntent.h pcre.h stdint.h stdio.h stdlib.h string.h sys/stat.h sys/statvfs.h sys/types.h sys/vfs.h ],[],[probe_partition_req_deps_ok=no; probe_partition_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of optional headers for the partition probe'
AC_CHECK_HEADERS([blkid/blkid.h linux/magic.h ],[],[probe_partition_opt_deps_ok=no],[-])

echo
echo ' * Checking presence of required headers for the inetlisteningservers probe'
AC_CHECK_HEADERS([arpa/inet.h dirent.h errno.h fcntl.h netdb.h regex.h stdio_ext.h stdio.h stdlib.h string.h ],[],[probe_inetlisteningservers_req_deps_ok=no; probe_inetlisteningservers_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the iflisteners probe'
AC_CHECK_HEADERS([arpa/inet.h dirent.h errno.h fcntl.h netdb.h regex.h stdio_ext.h stdio.h stdlib.h string.h ],[],[probe_iflisteners_req_deps_ok=no; probe_iflisteners_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the selinuxboolean probe'
AC_CHECK_HEADERS([errno.h fcntl.h selinux/selinux.h stdio.h stdlib.h string.h sys/stat.h ],[],[probe_selinuxboolean_req_deps_ok=no; probe_selinuxboolean_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the selinuxsecuritycontext probe'
AC_CHECK_HEADERS([dirent.h errno.h fcntl.h limits.h pthread.h selinux/context.h selinux/selinux.h stdlib.h string.h sys/stat.h sys/types.h ],[],[probe_selinuxsecuritycontext_req_deps_ok=no; probe_selinuxsecuritycontext_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the rpminfo probe'
AC_CHECK_HEADERS([assert.h errno.h fcntl.h regex.h rpm/header.h rpm/rpmdb.h rpm/rpmlib.h rpm/rpmlog.h rpm/rpmmacro.h rpm/rpmts.h stdio.h string.h sys/stat.h sys/types.h ],[],[probe_rpminfo_req_deps_ok=no; probe_rpminfo_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the rpmverify probe'
AC_CHECK_HEADERS([assert.h errno.h fcntl.h limits.h pcre.h rpm/header.h rpm/rpmcli.h rpm/rpmdb.h rpm/rpmfi.h rpm/rpmlib.h rpm/rpmlog.h rpm/rpmmacro.h rpm/rpmts.h stdio.h string.h sys/stat.h sys/types.h ],[],[probe_rpmverify_req_deps_ok=no; probe_rpmverify_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the rpmverifyfile probe'
AC_CHECK_HEADERS([assert.h errno.h fcntl.h limits.h pcre.h rpm/header.h rpm/rpmcli.h rpm/rpmdb.h rpm/rpmfi.h rpm/rpmlib.h rpm/rpmlog.h rpm/rpmmacro.h rpm/rpmts.h stdio.h string.h sys/stat.h sys/types.h ],[],[probe_rpmverifyfile_req_deps_ok=no; probe_rpmverifyfile_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the rpmverifypackage probe'
AC_CHECK_HEADERS([assert.h errno.h fcntl.h limits.h pcre.h popt.h rpm/header.h rpm/rpmcli.h rpm/rpmdb.h rpm/rpmfi.h rpm/rpmlib.h rpm/rpmlog.h rpm/rpmmacro.h rpm/rpmts.h stdio.h string.h sys/stat.h sys/types.h ],[],[probe_rpmverifypackage_req_deps_ok=no; probe_rpmverifypackage_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the dpkginfo probe'
AC_CHECK_HEADERS([assert.h errno.h stdio.h string.h ],[],[probe_dpkginfo_req_deps_ok=no; probe_dpkginfo_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the systemdunitproperty probe'
AC_CHECK_HEADERS([dbus/dbus.h string.h ],[],[probe_systemdunitproperty_req_deps_ok=no; probe_systemdunitproperty_req_deps_missing='header files'],[-])

echo
echo ' * Checking presence of required headers for the systemdunitdependency probe'
AC_CHECK_HEADERS([dbus/dbus.h string.h ],[],[probe_systemdunitdependency_req_deps_ok=no; probe_systemdunitdependency_req_deps_missing='header files'],[-])

CPPFLAGS="$SAVE_CPPFLAGS"

echo
echo '* Checking for acl library used by:  file'
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([acl_init],[acl],[
acl_CFLAGS=;
acl_LIBS=-lacl;
],[
probe_file_opt_deps_ok=no;
probe_file_opt_deps_missing+=', acl lib';
],[])
AC_SUBST([acl_CFLAGS])
AC_SUBST([acl_LIBS])
LIBS=$SAVE_LIBS
SAVE_LIBS=$LIBS
LIBS=$acl_LIBS
AC_CHECK_FUNCS([acl_init], [], [])
AC_CHECK_FUNCS([acl_extended_file],[],[])
LIBS=$SAVE_LIBS
echo
echo '* Checking for apt_pkg library used by: dpkginfo '
PKG_CHECK_MODULES([apt_pkg], [libapt-pkg >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([pkgInitConfig],[apt-pkg],[
apt_pkg_CFLAGS=;
apt_pkg_LIBS=-lapt-pkg;
],[
probe_dpkginfo_req_deps_ok=no;
probe_dpkginfo_req_deps_missing+=', apt_pkg';
],[])
AC_SUBST([apt_pkg_CFLAGS])
AC_SUBST([apt_pkg_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$apt_pkg_LIBS
AC_LANG_PUSH([C++])
AC_CHECK_FUNCS([pkgInitConfig pkgInitSystem], [], [
probe_dpkginfo_req_deps_ok=no;
probe_dpkginfo_req_deps_missing+=", $ac_func func";
])
AC_LANG_POP([C++])
LIBS=$SAVE_LIBS
echo
echo '* Checking for blkid library used by:  partition'
PKG_CHECK_MODULES([blkid], [blkid >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([blkid_get_cache],[blkid],[
blkid_CFLAGS=;
blkid_LIBS=-lblkid;
],[
probe_partition_opt_deps_ok=no;
probe_partition_opt_deps_missing+=', blkid';
],[])
AC_SUBST([blkid_CFLAGS])
AC_SUBST([blkid_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$blkid_LIBS
AC_CHECK_FUNCS([blkid_get_cache blkid_get_tag_value], [], [])
LIBS=$SAVE_LIBS
echo
echo '* Checking for cap library used by: process58 '
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([cap_init],[cap],[
cap_CFLAGS=;
cap_LIBS=-lcap;
],[
probe_process58_req_deps_ok=no;
probe_process58_req_deps_missing+=', cap lib';
],[])
AC_SUBST([cap_CFLAGS])
AC_SUBST([cap_LIBS])
LIBS=$SAVE_LIBS
SAVE_LIBS=$LIBS
LIBS=$cap_LIBS
AC_CHECK_FUNCS([cap_init], [], [
probe_process58_req_deps_ok=no;
probe_process58_req_deps_missing+=", $ac_func func";
])
AC_CHECK_FUNCS([cap_get_pid capgetp],[],[])
LIBS=$SAVE_LIBS
echo
echo '* Checking for dbus1 library used by: systemdunitproperty systemdunitproperty '
PKG_CHECK_MODULES([dbus1], [dbus-1 >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([dbus_bus_get],[dbus1],[
dbus1_CFLAGS=;
dbus1_LIBS=-ldbus1;
],[
probe_systemdunitproperty_req_deps_ok=no;
probe_systemdunitproperty_req_deps_missing+=', dbus1';
probe_systemdunitproperty_req_deps_ok=no;
probe_systemdunitproperty_req_deps_missing+=', dbus1';
],[])
AC_SUBST([dbus1_CFLAGS])
AC_SUBST([dbus1_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$dbus1_LIBS
AC_CHECK_FUNCS([dbus_bus_get], [], [
probe_systemdunitproperty_req_deps_ok=no;
probe_systemdunitproperty_req_deps_missing+=", $ac_func func";

probe_systemdunitproperty_req_deps_ok=no;
probe_systemdunitproperty_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for gconf2 library used by: gconf '
PKG_CHECK_MODULES([gconf2], [gconf-2.0 >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([gconf_engine_get_default],[gconf-2],[
gconf2_CFLAGS=;
gconf2_LIBS=-lgconf-2;
],[
probe_gconf_req_deps_ok=no;
probe_gconf_req_deps_missing+=', gconf2';
],[])
AC_SUBST([gconf2_CFLAGS])
AC_SUBST([gconf2_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$gconf2_LIBS
AC_CHECK_FUNCS([gconf_engine_get_default], [], [
probe_gconf_req_deps_ok=no;
probe_gconf_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for lber library used by: ldap57 '
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([ber_init],[lber],[
lber_CFLAGS=;
lber_LIBS=-llber;
],[
probe_ldap57_req_deps_ok=no;
probe_ldap57_req_deps_missing+=', lber lib';
],[])
AC_SUBST([lber_CFLAGS])
AC_SUBST([lber_LIBS])
LIBS=$SAVE_LIBS
SAVE_LIBS=$LIBS
LIBS=$lber_LIBS
AC_CHECK_FUNCS([ber_init], [], [
probe_ldap57_req_deps_ok=no;
probe_ldap57_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for ldap library used by: ldap57 '
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([ldap_init],[ldap],[
ldap_CFLAGS=;
ldap_LIBS=-lldap;
],[
probe_ldap57_req_deps_ok=no;
probe_ldap57_req_deps_missing+=', ldap lib';
],[])
AC_SUBST([ldap_CFLAGS])
AC_SUBST([ldap_LIBS])
LIBS=$SAVE_LIBS
SAVE_LIBS=$LIBS
LIBS=$ldap_LIBS
AC_CHECK_FUNCS([ldap_init], [], [
probe_ldap57_req_deps_ok=no;
probe_ldap57_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for opendbx library used by: sql sql57 '
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([odbx_init],[opendbx],[
opendbx_CFLAGS=;
opendbx_LIBS=-lopendbx;
],[
probe_sql_req_deps_ok=no;
probe_sql_req_deps_missing+=', opendbx lib';
probe_sql57_req_deps_ok=no;
probe_sql57_req_deps_missing+=', opendbx lib';
],[])
AC_SUBST([opendbx_CFLAGS])
AC_SUBST([opendbx_LIBS])
LIBS=$SAVE_LIBS
SAVE_LIBS=$LIBS
LIBS=$opendbx_LIBS
AC_CHECK_FUNCS([odbx_init], [], [
probe_sql_req_deps_ok=no;
probe_sql_req_deps_missing+=", $ac_func func";

probe_sql57_req_deps_ok=no;
probe_sql57_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for pcre library used by: textfilecontent54 textfilecontent partition '
PKG_CHECK_MODULES([pcre], [libpcre >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([pcre_exec],[pcre],[
pcre_CFLAGS=;
pcre_LIBS=-lpcre;
],[
probe_textfilecontent54_req_deps_ok=no;
probe_textfilecontent54_req_deps_missing+=', pcre';
probe_textfilecontent_req_deps_ok=no;
probe_textfilecontent_req_deps_missing+=', pcre';
probe_partition_req_deps_ok=no;
probe_partition_req_deps_missing+=', pcre';
],[])
AC_SUBST([pcre_CFLAGS])
AC_SUBST([pcre_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$pcre_LIBS
AC_CHECK_FUNCS([pcre_exec], [], [
probe_textfilecontent54_req_deps_ok=no;
probe_textfilecontent54_req_deps_missing+=", $ac_func func";

probe_textfilecontent_req_deps_ok=no;
probe_textfilecontent_req_deps_missing+=", $ac_func func";

probe_partition_req_deps_ok=no;
probe_partition_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for procps library used by:  process58 process'
PKG_CHECK_MODULES([procps], [libprocps >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([dev_to_tty],[procps-ng],[
procps_CFLAGS=;
procps_LIBS=-lprocps-ng;
],[
probe_process58_opt_deps_ok=no;
probe_process58_opt_deps_missing+=', procps';
probe_process_opt_deps_ok=no;
probe_process_opt_deps_missing+=', procps';
],[])
AC_SUBST([procps_CFLAGS])
AC_SUBST([procps_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$procps_LIBS
AC_CHECK_FUNCS([dev_to_tty], [], [])
LIBS=$SAVE_LIBS
echo
echo '* Checking for rpm library used by: rpminfo rpmverify rpmverifyfile rpmverifypackage '
PKG_CHECK_MODULES([rpm], [rpm >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([rpmtsCreate],[rpm],[
rpm_CFLAGS=;
rpm_LIBS=-lrpm;
],[
probe_rpminfo_req_deps_ok=no;
probe_rpminfo_req_deps_missing+=', rpm';
probe_rpmverify_req_deps_ok=no;
probe_rpmverify_req_deps_missing+=', rpm';
probe_rpmverifyfile_req_deps_ok=no;
probe_rpmverifyfile_req_deps_missing+=', rpm';
probe_rpmverifypackage_req_deps_ok=no;
probe_rpmverifypackage_req_deps_missing+=', rpm';
],[])
AC_SUBST([rpm_CFLAGS])
AC_SUBST([rpm_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$rpm_LIBS
AC_CHECK_FUNCS([rpmtsCreate rpmReadConfigFiles], [], [
probe_rpminfo_req_deps_ok=no;
probe_rpminfo_req_deps_missing+=", $ac_func func";

probe_rpmverify_req_deps_ok=no;
probe_rpmverify_req_deps_missing+=", $ac_func func";

probe_rpmverifyfile_req_deps_ok=no;
probe_rpmverifyfile_req_deps_missing+=", $ac_func func";

probe_rpmverifypackage_req_deps_ok=no;
probe_rpmverifypackage_req_deps_missing+=", $ac_func func";
])
AC_CHECK_FUNCS([headerFormat headerSprintf rpmFreeCrypto rpmFreeFilesystems],[],[])
LIBS=$SAVE_LIBS
echo
echo '* Checking for selinux library used by: process58 selinuxboolean selinuxsecuritycontext '
PKG_CHECK_MODULES([selinux], [libselinux >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([security_get_boolean_names],[selinux],[
selinux_CFLAGS=;
selinux_LIBS=-lselinux;
],[
probe_process58_req_deps_ok=no;
probe_process58_req_deps_missing+=', selinux';
probe_selinuxboolean_req_deps_ok=no;
probe_selinuxboolean_req_deps_missing+=', selinux';
probe_selinuxsecuritycontext_req_deps_ok=no;
probe_selinuxsecuritycontext_req_deps_missing+=', selinux';
],[])
AC_SUBST([selinux_CFLAGS])
AC_SUBST([selinux_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$selinux_LIBS
AC_CHECK_FUNCS([security_get_boolean_names], [], [
probe_process58_req_deps_ok=no;
probe_process58_req_deps_missing+=", $ac_func func";

probe_selinuxboolean_req_deps_ok=no;
probe_selinuxboolean_req_deps_missing+=", $ac_func func";

probe_selinuxsecuritycontext_req_deps_ok=no;
probe_selinuxsecuritycontext_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for xml2 library used by: xmlfilecontent '
PKG_CHECK_MODULES([xml2], [libxml-2.0 >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([xmlTextReaderRead],[xml2],[
xml2_CFLAGS=;
xml2_LIBS=-lxml2;
],[
probe_xmlfilecontent_req_deps_ok=no;
probe_xmlfilecontent_req_deps_missing+=', xml2';
],[])
AC_SUBST([xml2_CFLAGS])
AC_SUBST([xml2_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$xml2_LIBS
AC_CHECK_FUNCS([xmlTextReaderRead], [], [
probe_xmlfilecontent_req_deps_ok=no;
probe_xmlfilecontent_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo
echo '* Checking for xslt library used by: xmlfilecontent '
PKG_CHECK_MODULES([xslt], [libxslt >= 0.0],[],[
SAVE_LIBS=$LIBS
AC_SEARCH_LIBS([xsltDocumentFunction],[xslt],[
xslt_CFLAGS=;
xslt_LIBS=-lxslt;
],[
probe_xmlfilecontent_req_deps_ok=no;
probe_xmlfilecontent_req_deps_missing+=', xslt';
],[])
AC_SUBST([xslt_CFLAGS])
AC_SUBST([xslt_LIBS])
LIBS=$SAVE_LIBS
])
SAVE_LIBS=$LIBS
LIBS=$xslt_LIBS
AC_CHECK_FUNCS([xsltDocumentFunction], [], [
probe_xmlfilecontent_req_deps_ok=no;
probe_xmlfilecontent_req_deps_missing+=", $ac_func func";
])
LIBS=$SAVE_LIBS
echo


#check for atomic functions
case $host_cpu in
	i386 | i486 | i586 | i686)
		CFLAGS="$CFLAGS  -march=i686"
		;;
esac

AC_CACHE_CHECK([for atomic builtins], [ac_cv_atomic_builtins],
[AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <stdint.h>
				  uint16_t foovar=0; uint16_t old=1; uint16_t new=2;],
				[__sync_bool_compare_and_swap(&foovar,old,new); return __sync_fetch_and_add(&foovar, 1);])],
		[ac_cv_atomic_builtins=yes],
		[ac_cv_atomic_builtins=no])])
if test $ac_cv_atomic_builtins = yes; then
  AC_DEFINE([HAVE_ATOMIC_BUILTINS], 1, [Define to 1 if the compiler supports atomic builtins.])
else
  AC_MSG_NOTICE([!!! Compiler does not support atomic builtins. Atomic operation will be emulated using mutex-based locking. !!!])
fi


AC_ARG_ENABLE([probes-independent],
     [AC_HELP_STRING([--enable-probes-independent], [enable compilation of probes independent of the base system (default=yes)])],
     [case "${enableval}" in
       yes) probes_independent=yes ;;
       no)  probes_independent=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes-independent]) ;;
     esac],[probes_independent=yes])

AC_ARG_ENABLE([probes-unix],
     [AC_HELP_STRING([--enable-probes-unix], [enable compilation of probes for UNIX based systems (default=yes)])],
     [case "${enableval}" in
       yes) probes_unix=yes ;;
       no)  probes_unix=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes-unix]) ;;
     esac],[probes_unix=yes])
if test "x${probes_unix}" = xyes; then
	AC_DEFINE([PLATFORM_UNIX], [1], [Indicator for a Unix type OS])
fi


probes_linux=no
case "${host}" in
    *-*-linux*)
        probes_linux=yes
    ;;
esac
AC_ARG_ENABLE([probes-linux],
     [AC_HELP_STRING([--enable-probes-linux], [enable compilation of probes for Linux based systems (default=autodetect)])],
     [case "${enableval}" in
       yes) probes_linux=yes ;;
       no)  probes_linux=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes-linux]) ;;
     esac],)

probes_solaris=no
case "${host}" in
    *-*-solaris*)
        probes_solaris=yes
    ;;
esac
AC_ARG_ENABLE([probes-solaris],
     [AC_HELP_STRING([--enable-probes-solaris], [enable compilation of probes for Solaris based systems (default=autodetect)])],
     [case "${enableval}" in
       yes) probes_solaris=yes ;;
       no)  probes_solaris=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes-solaris]) ;;
     esac],)

AC_ARG_ENABLE([cce],
     [AC_HELP_STRING([--enable-cce], [include support for CCE (default=no)])],
     [case "${enableval}" in
       yes) cce=yes ;;
       no)  cce=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cce]) ;;
     esac],[cce=no])

AC_ARG_ENABLE([python],
     [AC_HELP_STRING([--enable-python], [enable compilation of python2 bindings (default=yes)])],
     [case "${enableval}" in
       yes) python_bind=yes ;;
       no)  python_bind=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-python]) ;;
     esac],[python_bind=yes])

AC_ARG_ENABLE([python3],
	[AC_HELP_STRING([--enable-python3], [enable compilation of python3 bindings (default=no)])],
	[case "${enableval}" in
		yes) python3_bind=yes ;;
		no) python3_bind=no ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-python3]);;
	esac],[python3_bind=no])

AC_ARG_ENABLE([perl],
     [AC_HELP_STRING([--enable-perl], [enable compilation of perl bindings (default=no)])],
     [case "${enableval}" in
       yes) perl_bind=yes ;;
       no)  perl_bind=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-perl]) ;;
     esac],[perl_bind=no])

AC_ARG_ENABLE([regex-posix],
     [AC_HELP_STRING([--enable-regex-posix], [compile with POSIX instead of PCRE regex (default=no)])],
     [case "${enableval}" in
       yes) regex_posix=yes ;;
       no)  regex_posix=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-regex-posix]) ;;
     esac],[regex_posix=no])

AC_ARG_ENABLE([debug],
     [AC_HELP_STRING([--enable-debug], [enable debugging flags (default=no)])],
     [case "${enableval}" in
       yes) debug=yes ;;
       no)  debug=no ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac], [debug=no])

AC_ARG_ENABLE([valgrind],
     [AC_HELP_STRING([--enable-valgrind], [enable valgrind checks (default=no)])],
     [case "${enableval}" in
       yes) vgdebug=yes ;;
       no)  vgdebug=no ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-valgrind]) ;;
     esac], [vgdebug=no])


AC_ARG_ENABLE([ssp],
     [AC_HELP_STRING([--enable-ssp], [enable SSP (fstack-protector, default=no)])],
     [case "${enableval}" in
       yes) ssp=yes ;;
       no)  ssp=no ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-ssp]) ;;
     esac], [ssp=no])

AC_ARG_WITH([crypto],
     [AS_HELP_STRING([--with-crypto],
     [use different crypto backend. Available options: nss3, gcrypt [default=gcrypt]])],
     [],
     [crypto=gcrypt])

if test "x${libexecdir}" = xNONE; then
	probe_dir="/usr/local/libexec/openscap"
else
	EXPAND_DIR(probe_dir,"${libexecdir}/openscap")
fi

AC_SUBST(probe_dir)

if test "x${prefix}" = xNONE; then
	AC_DEFINE_UNQUOTED([OSCAP_DEFAULT_SCHEMA_PATH], ["/usr/local/share/openscap/schemas"], [Path to xml schemas])
else
	AC_DEFINE_UNQUOTED([OSCAP_DEFAULT_SCHEMA_PATH], ["${prefix}/share/openscap/schemas"], [Path to xml schemas])
fi

if test "x${prefix}" = xNONE; then
	AC_DEFINE_UNQUOTED([OSCAP_DEFAULT_XSLT_PATH], ["/usr/local/share/openscap/xsl"], [Path to xslt files])
else
	AC_DEFINE_UNQUOTED([OSCAP_DEFAULT_XSLT_PATH], ["${prefix}/share/openscap/xsl"], [Path to xslt files])
fi

if test "x${prefix}" = xNONE; then
	AC_DEFINE_UNQUOTED([OSCAP_DEFAULT_CPE_PATH], ["/usr/local/share/openscap/cpe"], [Path to cpe files])
else
	AC_DEFINE_UNQUOTED([OSCAP_DEFAULT_CPE_PATH], ["${prefix}/share/openscap/cpe"], [Path to cpe files])
fi

if test "$regex_posix" = "yes"; then
   AC_DEFINE([USE_REGEX_POSIX], [1], [Use POSIX regular expressions])
else
   AC_DEFINE([USE_REGEX_PCRE], [1], [Use PCRE])
fi

if test "$ssp" = "yes"; then
   GCC_STACK_PROTECT_CC
   GCC_STACK_PROTECT_CXX
fi

if test "$debug" = "yes"; then
   CFLAGS="$CFLAGS $CFLAGS_DEBUGGING"
else
   CFLAGS="$CFLAGS $CFLAGS_NODEBUG"
   AC_DEFINE([NDEBUG], [1], [No Debug defined])
fi

AC_ARG_ENABLE([sce],
     [AC_HELP_STRING([--enable-sce], [enable script check engine (default=no)])],
     [case "${enableval}" in
       yes) sce=yes ;;
       no)  sce=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-sce]) ;;
     esac],[sce=no])

if test "x${sce}" = xyes; then
  AC_DEFINE([ENABLE_SCE], [1], [compilation of script check engine enabled])
fi

AC_ARG_ENABLE([util-oscap],
     [AC_HELP_STRING([--enable-util-oscap], [enable compilation of the oscap utility (default=yes)])],
     [case "${enableval}" in
       yes) util_oscap=yes ;;
       no)  util_oscap=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-util-oscap]) ;;
     esac],[util_oscap=yes])

AC_ARG_ENABLE([util-scap-as-rpm],
     [AC_HELP_STRING([--enable-util-scap-as-rpm], [enable compilation of the scap-as-rpm utility (default=yes)])],
     [case "${enableval}" in
       yes) util_scap_as_rpm=yes ;;
       no)  util_scap_as_rpm=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-util-scap-as-rpm]) ;;
     esac],[util_scap_as_rpm=yes])

if test "$vgdebug" = "yes"; then
 if test "$HAVE_VALGRIND" = "yes"; then
   vgcheck="yes"
 else
   vgcheck="valgrind not installed"
 fi
else
   vgcheck="no"
fi

if test "x${perl_bind}" = xyes; then
	AC_PATH_PROG(PERL, perl)
	PERL_INCLUDES="`$PERL -e 'use Config; print $Config{archlib}'`/CORE"
	vendorlib="$(  $PERL -e 'use Config; print $Config{vendorlib}'  | sed "s|$($PERL -e 'use Config; print $Config{prefix}')||" )"
	vendorarch="$( $PERL -e 'use Config; print $Config{vendorarch}' | sed "s|$($PERL -e 'use Config; print $Config{prefix}')||" )"
	AC_SUBST([PERL_INCLUDES], ["-I$PERL_INCLUDES"])
	AC_SUBST([perl_vendorlibdir], ['${prefix}'$vendorlib])
	AC_SUBST([perl_vendorarchdir], ['${prefix}'$vendorarch])
	save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $PERL_INCLUDES"
	AC_CHECK_HEADERS([EXTERN.h],[],[AC_MSG_ERROR(Perl development librarier are needed for perl bindings)],[-])
	CPPFLAGS="$save_CPPFLAGS"
fi

# (AM_PATH_PYTHON) cannot be used for multiple Python version at once
if test "x${python3_bind}" = xyes; then
	AC_PATH_PROG([PYTHON3], [python3])
	AC_PATH_PROG([PYTHON3_CONFIG], [python3-config], [no])
		[if test "$PYTHON3_CONFIG" = "no"]
		[then]
			[echo "The python3-config program was not found in the search path. Please ensure"]
			[echo "that it is installed and its directory is included in the search path."]
			[echo "Then run configure again before attempting to build OpenSCAP."]
			[exit 1]
		[fi]
	PYTHON3_CFLAGS=`python3-config --cflags 2> /dev/null`
	PYTHON3_LIBS=`python3-config --libs 2> /dev/null`
	PYTHON3_INCLUDES=`python3-config --includes 2> /dev/null`

	# the string concatenation below is just a trick to prevent substitution
	PYTHON3_DIR=`$PYTHON3 -c "import distutils.sysconfig; \
		print(distutils.sysconfig.get_python_lib(0,0,prefix='$' '{prefix}'))"`
	PYTHON3_EXECDIR=`$PYTHON3 -c "import distutils.sysconfig; \
		print(distutils.sysconfig.get_python_lib(1,0,prefix='$' '{exec_prefix}'))"`

	AC_SUBST(PYTHON3_CFLAGS)
	AC_SUBST(PYTHON3_LIBS)
	AC_SUBST(PYTHON3_INCLUDES)
	AC_SUBST(python3dir, $PYTHON3_DIR)
	AC_SUBST(py3execdir, $PYTHON3_EXECDIR)
fi


AM_CONDITIONAL([probe_system_info_enabled], test "$probe_system_info_req_deps_ok" = yes)
probe_system_info_enabled=$probe_system_info_req_deps_ok
AM_CONDITIONAL([probe_family_enabled], test "$probe_family_req_deps_ok" = yes)
probe_family_enabled=$probe_family_req_deps_ok
AM_CONDITIONAL([probe_textfilecontent_enabled], test "$probe_textfilecontent_req_deps_ok" = yes)
probe_textfilecontent_enabled=$probe_textfilecontent_req_deps_ok
AM_CONDITIONAL([probe_textfilecontent54_enabled], test "$probe_textfilecontent54_req_deps_ok" = yes)
probe_textfilecontent54_enabled=$probe_textfilecontent54_req_deps_ok
AM_CONDITIONAL([probe_variable_enabled], test "$probe_variable_req_deps_ok" = yes)
probe_variable_enabled=$probe_variable_req_deps_ok
AM_CONDITIONAL([probe_xmlfilecontent_enabled], test "$probe_xmlfilecontent_req_deps_ok" = yes)
probe_xmlfilecontent_enabled=$probe_xmlfilecontent_req_deps_ok
AM_CONDITIONAL([probe_filehash_enabled], test "$probe_filehash_req_deps_ok" = yes)
probe_filehash_enabled=$probe_filehash_req_deps_ok
AM_CONDITIONAL([probe_filehash58_enabled], test "$probe_filehash58_req_deps_ok" = yes)
probe_filehash58_enabled=$probe_filehash58_req_deps_ok
AM_CONDITIONAL([probe_environmentvariable_enabled], test "$probe_environmentvariable_req_deps_ok" = yes)
probe_environmentvariable_enabled=$probe_environmentvariable_req_deps_ok
AM_CONDITIONAL([probe_environmentvariable58_enabled], test "$probe_environmentvariable58_req_deps_ok" = yes)
probe_environmentvariable58_enabled=$probe_environmentvariable58_req_deps_ok
AM_CONDITIONAL([probe_sql_enabled], test "$probe_sql_req_deps_ok" = yes)
probe_sql_enabled=$probe_sql_req_deps_ok
AM_CONDITIONAL([probe_sql57_enabled], test "$probe_sql57_req_deps_ok" = yes)
probe_sql57_enabled=$probe_sql57_req_deps_ok
AM_CONDITIONAL([probe_ldap57_enabled], test "$probe_ldap57_req_deps_ok" = yes)
probe_ldap57_enabled=$probe_ldap57_req_deps_ok
AM_CONDITIONAL([probe_dnscache_enabled], test "$probe_dnscache_req_deps_ok" = yes)
probe_dnscache_enabled=$probe_dnscache_req_deps_ok
AM_CONDITIONAL([probe_runlevel_enabled], test "$probe_runlevel_req_deps_ok" = yes)
probe_runlevel_enabled=$probe_runlevel_req_deps_ok
AM_CONDITIONAL([probe_file_enabled], test "$probe_file_req_deps_ok" = yes)
probe_file_enabled=$probe_file_req_deps_ok
AM_CONDITIONAL([probe_fileextendedattribute_enabled], test "$probe_fileextendedattribute_req_deps_ok" = yes)
probe_fileextendedattribute_enabled=$probe_fileextendedattribute_req_deps_ok
AM_CONDITIONAL([probe_password_enabled], test "$probe_password_req_deps_ok" = yes)
probe_password_enabled=$probe_password_req_deps_ok
AM_CONDITIONAL([probe_process_enabled], test "$probe_process_req_deps_ok" = yes)
probe_process_enabled=$probe_process_req_deps_ok
AM_CONDITIONAL([probe_process58_enabled], test "$probe_process58_req_deps_ok" = yes)
probe_process58_enabled=$probe_process58_req_deps_ok
AM_CONDITIONAL([probe_shadow_enabled], test "$probe_shadow_req_deps_ok" = yes)
probe_shadow_enabled=$probe_shadow_req_deps_ok
AM_CONDITIONAL([probe_uname_enabled], test "$probe_uname_req_deps_ok" = yes)
probe_uname_enabled=$probe_uname_req_deps_ok
AM_CONDITIONAL([probe_interface_enabled], test "$probe_interface_req_deps_ok" = yes)
probe_interface_enabled=$probe_interface_req_deps_ok
AM_CONDITIONAL([probe_xinetd_enabled], test "$probe_xinetd_req_deps_ok" = yes)
probe_xinetd_enabled=$probe_xinetd_req_deps_ok
AM_CONDITIONAL([probe_sysctl_enabled], test "$probe_sysctl_req_deps_ok" = yes)
probe_sysctl_enabled=$probe_sysctl_req_deps_ok
AM_CONDITIONAL([probe_routingtable_enabled], test "$probe_routingtable_req_deps_ok" = yes)
probe_routingtable_enabled=$probe_routingtable_req_deps_ok
AM_CONDITIONAL([probe_gconf_enabled], test "$probe_gconf_req_deps_ok" = yes)
probe_gconf_enabled=$probe_gconf_req_deps_ok
AM_CONDITIONAL([probe_isainfo_enabled], test "$probe_isainfo_req_deps_ok" = yes)
probe_isainfo_enabled=$probe_isainfo_req_deps_ok
AM_CONDITIONAL([probe_package_enabled], test "$probe_package_req_deps_ok" = yes)
probe_package_enabled=$probe_package_req_deps_ok
AM_CONDITIONAL([probe_patch_enabled], test "$probe_patch_req_deps_ok" = yes)
probe_patch_enabled=$probe_patch_req_deps_ok
AM_CONDITIONAL([probe_smf_enabled], test "$probe_smf_req_deps_ok" = yes)
probe_smf_enabled=$probe_smf_req_deps_ok
AM_CONDITIONAL([probe_partition_enabled], test "$probe_partition_req_deps_ok" = yes)
probe_partition_enabled=$probe_partition_req_deps_ok
AM_CONDITIONAL([probe_inetlisteningservers_enabled], test "$probe_inetlisteningservers_req_deps_ok" = yes)
probe_inetlisteningservers_enabled=$probe_inetlisteningservers_req_deps_ok
AM_CONDITIONAL([probe_iflisteners_enabled], test "$probe_iflisteners_req_deps_ok" = yes)
probe_iflisteners_enabled=$probe_iflisteners_req_deps_ok
AM_CONDITIONAL([probe_selinuxboolean_enabled], test "$probe_selinuxboolean_req_deps_ok" = yes)
probe_selinuxboolean_enabled=$probe_selinuxboolean_req_deps_ok
AM_CONDITIONAL([probe_selinuxsecuritycontext_enabled], test "$probe_selinuxsecuritycontext_req_deps_ok" = yes)
probe_selinuxsecuritycontext_enabled=$probe_selinuxsecuritycontext_req_deps_ok
AM_CONDITIONAL([probe_rpminfo_enabled], test "$probe_rpminfo_req_deps_ok" = yes)
probe_rpminfo_enabled=$probe_rpminfo_req_deps_ok
AM_CONDITIONAL([probe_rpmverify_enabled], test "$probe_rpmverify_req_deps_ok" = yes)
probe_rpmverify_enabled=$probe_rpmverify_req_deps_ok
AM_CONDITIONAL([probe_rpmverifyfile_enabled], test "$probe_rpmverifyfile_req_deps_ok" = yes)
probe_rpmverifyfile_enabled=$probe_rpmverifyfile_req_deps_ok
AM_CONDITIONAL([probe_rpmverifypackage_enabled], test "$probe_rpmverifypackage_req_deps_ok" = yes)
probe_rpmverifypackage_enabled=$probe_rpmverifypackage_req_deps_ok
AM_CONDITIONAL([probe_dpkginfo_enabled], test "$probe_dpkginfo_req_deps_ok" = yes)
probe_dpkginfo_enabled=$probe_dpkginfo_req_deps_ok
AM_CONDITIONAL([probe_systemdunitproperty_enabled], test "$probe_systemdunitproperty_req_deps_ok" = yes)
probe_systemdunitproperty_enabled=$probe_systemdunitproperty_req_deps_ok
AM_CONDITIONAL([probe_systemdunitdependency_enabled], test "$probe_systemdunitdependency_req_deps_ok" = yes)
probe_systemdunitdependency_enabled=$probe_systemdunitdependency_req_deps_ok

AM_CONDITIONAL([WANT_CCE],  test "$cce"  = yes)

AM_CONDITIONAL([WANT_PROBES_INDEPENDENT], test "$probes_independent" = yes)
AM_CONDITIONAL([WANT_PROBES_UNIX], test "$probes_unix" = yes)
AM_CONDITIONAL([WANT_PROBES_LINUX], test "$probes_linux" = yes)
AM_CONDITIONAL([WANT_PROBES_SOLARIS], test "$probes_solaris" = yes)

AM_CONDITIONAL([WANT_SCE], test "$sce" = yes)
AM_CONDITIONAL([WANT_UTIL_OSCAP], test "$util_oscap" = yes)
AM_CONDITIONAL([WANT_UTIL_SCAP_AS_RPM], test "$util_scap_as_rpm" = yes)
AM_CONDITIONAL([WANT_PYTHON], test "$python_bind" = yes)
AM_CONDITIONAL([WANT_PYTHON3], test "$python3_bind" = yes)
AM_CONDITIONAL([WANT_PERL], test "$perl_bind" = yes)
AM_CONDITIONAL([ENABLE_VALGRIND_TESTS], test "$vgcheck" = yes)

#
# Core
#
AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 xsl/Makefile
                 schemas/Makefile
                 cpe/Makefile
                 libopenscap.pc
                 src/common/Makefile
                 tests/Makefile
                 tests/API/Makefile

                 swig/Makefile
		swig/perl/Makefile
		swig/python2/Makefile
		swig/python3/Makefile

                 utils/Makefile

                 src/OVAL/Makefile
		src/OVAL/adt/Makefile
		src/OVAL/results/Makefile
                 tests/API/OVAL/Makefile
		tests/oscap_string/Makefile
                 tests/API/OVAL/unittests/Makefile
		 tests/API/OVAL/validate/Makefile
		 tests/API/OVAL/report_variable_values/Makefile
                 tests/mitre/Makefile

                 src/OVAL/probes/Makefile
                 src/OVAL/probes/probe/Makefile
                 src/OVAL/probes/crapi/Makefile
                 src/OVAL/probes/SEAP/Makefile
                 src/OVAL/probes/SEAP/generic/rbt/Makefile
                 tests/probes/Makefile
                 tests/API/crypt/Makefile
                 tests/API/SEAP/Makefile
                 tests/API/probes/Makefile
                 tests/probes/file/Makefile
                 tests/probes/fileextendedattribute/Makefile
                 tests/probes/uname/Makefile
                 tests/probes/shadow/Makefile
		tests/probes/sql57/Makefile
                 tests/probes/family/Makefile
                 tests/probes/process58/Makefile
                 tests/probes/sysinfo/Makefile
                 tests/probes/rpminfo/Makefile
		 tests/probes/rpmverify/Makefile
                 tests/probes/systemdunitproperty/Makefile
                 tests/probes/systemdunitdependency/Makefile
                 tests/probes/runlevel/Makefile
                 tests/probes/filehash/Makefile
                 tests/probes/filehash58/Makefile
                 tests/probes/password/Makefile
                 tests/probes/interface/Makefile
                 tests/probes/textfilecontent54/Makefile
                 tests/probes/environmentvariable/Makefile
                 tests/probes/environmentvariable58/Makefile
                 tests/probes/xinetd/Makefile
                 tests/probes/selinuxboolean/Makefile
                 tests/probes/isainfo/Makefile
                 tests/probes/iflisteners/Makefile
		 tests/probes/maskattr/Makefile

                 tests/probes/vgtest/Makefile

                 src/CVSS/Makefile
                 tests/API/CVSS/Makefile

                 src/CVE/Makefile
                 tests/API/CVE/Makefile

                 src/CPE/Makefile
                 tests/API/CPE/Makefile
                 tests/API/CPE/name/Makefile
                 tests/API/CPE/lang/Makefile
                 tests/API/CPE/dict/Makefile
                 tests/API/CPE/inbuilt/Makefile

                 src/CCE/Makefile
                 tests/API/CCE/Makefile

                 src/DS/Makefile
                 tests/DS/Makefile
                 tests/DS/ds_sds_index/Makefile
                 tests/DS/signed/Makefile
                 tests/DS/validate/Makefile

                 tests/bindings/Makefile

                 src/XCCDF/Makefile
                 src/XCCDF_POLICY/Makefile
                 tests/API/XCCDF/Makefile
                 tests/API/XCCDF/applicability/Makefile
                 tests/API/XCCDF/default_cpe/Makefile
                 tests/API/XCCDF/fix/Makefile
                 tests/API/XCCDF/guide/Makefile
                 tests/API/XCCDF/unittests/Makefile
                 tests/API/XCCDF/parser/Makefile
                 tests/API/XCCDF/progress/Makefile
                 tests/API/XCCDF/report/Makefile
                 tests/API/XCCDF/result_files/Makefile
                 tests/API/XCCDF/tailoring/Makefile
                 tests/API/XCCDF/variable_instance/Makefile

                 tests/schemas/Makefile
                 tests/oval_details/Makefile

                 src/SCE/Makefile
                 tests/sce/Makefile])

AC_CONFIG_FILES([run],
                [chmod +x,-w run])

AC_OUTPUT

echo "******************************************************"
echo "OpenSCAP will be compiled with the following settings:"
echo
echo "oscap tool:                    $util_oscap"
echo "scap-as-rpm tool:              $util_scap_as_rpm"
echo "python2 bindings enabled:      $python_bind"
echo "python3 bindings enabled:      $python3_bind"
echo "perl bindings enabled:         $perl_bind"
echo "use POSIX regex:               $regex_posix"
echo "SCE enabled                    $sce"
echo "debugging flags enabled:       $debug"
echo "CCE enabled:                   $cce"
echo
echo '  === probes ==='
if test "$probe_system_info_req_deps_ok" = "yes"; then
  probe_system_info_table_result="yes"
else
  probe_system_info_table_result="NO (missing: $probe_system_info_req_deps_missing)"
fi
printf "  %-28s %s\n" "system_info:" "$probe_system_info_table_result"
if test "$probe_family_req_deps_ok" = "yes"; then
  probe_family_table_result="yes"
else
  probe_family_table_result="NO (missing: $probe_family_req_deps_missing)"
fi
printf "  %-28s %s\n" "family:" "$probe_family_table_result"
if test "$probe_textfilecontent_req_deps_ok" = "yes"; then
  probe_textfilecontent_table_result="yes"
else
  probe_textfilecontent_table_result="NO (missing: $probe_textfilecontent_req_deps_missing)"
fi
printf "  %-28s %s\n" "textfilecontent:" "$probe_textfilecontent_table_result"
if test "$probe_textfilecontent54_req_deps_ok" = "yes"; then
  probe_textfilecontent54_table_result="yes"
else
  probe_textfilecontent54_table_result="NO (missing: $probe_textfilecontent54_req_deps_missing)"
fi
printf "  %-28s %s\n" "textfilecontent54:" "$probe_textfilecontent54_table_result"
if test "$probe_variable_req_deps_ok" = "yes"; then
  probe_variable_table_result="yes"
else
  probe_variable_table_result="NO (missing: $probe_variable_req_deps_missing)"
fi
printf "  %-28s %s\n" "variable:" "$probe_variable_table_result"
if test "$probe_xmlfilecontent_req_deps_ok" = "yes"; then
  probe_xmlfilecontent_table_result="yes"
else
  probe_xmlfilecontent_table_result="NO (missing: $probe_xmlfilecontent_req_deps_missing)"
fi
printf "  %-28s %s\n" "xmlfilecontent:" "$probe_xmlfilecontent_table_result"
if test "$probe_filehash_req_deps_ok" = "yes"; then
  probe_filehash_table_result="yes"
else
  probe_filehash_table_result="NO (missing: $probe_filehash_req_deps_missing)"
fi
printf "  %-28s %s\n" "filehash:" "$probe_filehash_table_result"
if test "$probe_filehash58_req_deps_ok" = "yes"; then
  probe_filehash58_table_result="yes"
else
  probe_filehash58_table_result="NO (missing: $probe_filehash58_req_deps_missing)"
fi
printf "  %-28s %s\n" "filehash58:" "$probe_filehash58_table_result"
if test "$probe_environmentvariable_req_deps_ok" = "yes"; then
  probe_environmentvariable_table_result="yes"
else
  probe_environmentvariable_table_result="NO (missing: $probe_environmentvariable_req_deps_missing)"
fi
printf "  %-28s %s\n" "environmentvariable:" "$probe_environmentvariable_table_result"
if test "$probe_environmentvariable58_req_deps_ok" = "yes"; then
  probe_environmentvariable58_table_result="yes"
else
  probe_environmentvariable58_table_result="NO (missing: $probe_environmentvariable58_req_deps_missing)"
fi
printf "  %-28s %s\n" "environmentvariable58:" "$probe_environmentvariable58_table_result"
if test "$probe_sql_req_deps_ok" = "yes"; then
  probe_sql_table_result="yes"
else
  probe_sql_table_result="NO (missing: $probe_sql_req_deps_missing)"
fi
printf "  %-28s %s\n" "sql:" "$probe_sql_table_result"
if test "$probe_sql57_req_deps_ok" = "yes"; then
  probe_sql57_table_result="yes"
else
  probe_sql57_table_result="NO (missing: $probe_sql57_req_deps_missing)"
fi
printf "  %-28s %s\n" "sql57:" "$probe_sql57_table_result"
if test "$probe_ldap57_req_deps_ok" = "yes"; then
  probe_ldap57_table_result="yes"
else
  probe_ldap57_table_result="NO (missing: $probe_ldap57_req_deps_missing)"
fi
printf "  %-28s %s\n" "ldap57:" "$probe_ldap57_table_result"
if test "$probe_dnscache_req_deps_ok" = "yes"; then
  probe_dnscache_table_result="yes"
else
  probe_dnscache_table_result="NO (missing: $probe_dnscache_req_deps_missing)"
fi
printf "  %-28s %s\n" "dnscache:" "$probe_dnscache_table_result"
if test "$probe_runlevel_req_deps_ok" = "yes"; then
  probe_runlevel_table_result="yes"
else
  probe_runlevel_table_result="NO (missing: $probe_runlevel_req_deps_missing)"
fi
printf "  %-28s %s\n" "runlevel:" "$probe_runlevel_table_result"
if test "$probe_file_req_deps_ok" = "yes"; then
  probe_file_table_result="yes"
else
  probe_file_table_result="NO (missing: $probe_file_req_deps_missing)"
fi
printf "  %-28s %s\n" "file:" "$probe_file_table_result"
if test "$probe_fileextendedattribute_req_deps_ok" = "yes"; then
  probe_fileextendedattribute_table_result="yes"
else
  probe_fileextendedattribute_table_result="NO (missing: $probe_fileextendedattribute_req_deps_missing)"
fi
printf "  %-28s %s\n" "fileextendedattribute:" "$probe_fileextendedattribute_table_result"
if test "$probe_password_req_deps_ok" = "yes"; then
  probe_password_table_result="yes"
else
  probe_password_table_result="NO (missing: $probe_password_req_deps_missing)"
fi
printf "  %-28s %s\n" "password:" "$probe_password_table_result"
if test "$probe_process_req_deps_ok" = "yes"; then
  probe_process_table_result="yes"
else
  probe_process_table_result="NO (missing: $probe_process_req_deps_missing)"
fi
printf "  %-28s %s\n" "process:" "$probe_process_table_result"
if test "$probe_process58_req_deps_ok" = "yes"; then
  probe_process58_table_result="yes"
else
  probe_process58_table_result="NO (missing: $probe_process58_req_deps_missing)"
fi
printf "  %-28s %s\n" "process58:" "$probe_process58_table_result"
if test "$probe_shadow_req_deps_ok" = "yes"; then
  probe_shadow_table_result="yes"
else
  probe_shadow_table_result="NO (missing: $probe_shadow_req_deps_missing)"
fi
printf "  %-28s %s\n" "shadow:" "$probe_shadow_table_result"
if test "$probe_uname_req_deps_ok" = "yes"; then
  probe_uname_table_result="yes"
else
  probe_uname_table_result="NO (missing: $probe_uname_req_deps_missing)"
fi
printf "  %-28s %s\n" "uname:" "$probe_uname_table_result"
if test "$probe_interface_req_deps_ok" = "yes"; then
  probe_interface_table_result="yes"
else
  probe_interface_table_result="NO (missing: $probe_interface_req_deps_missing)"
fi
printf "  %-28s %s\n" "interface:" "$probe_interface_table_result"
if test "$probe_xinetd_req_deps_ok" = "yes"; then
  probe_xinetd_table_result="yes"
else
  probe_xinetd_table_result="NO (missing: $probe_xinetd_req_deps_missing)"
fi
printf "  %-28s %s\n" "xinetd:" "$probe_xinetd_table_result"
if test "$probe_sysctl_req_deps_ok" = "yes"; then
  probe_sysctl_table_result="yes"
else
  probe_sysctl_table_result="NO (missing: $probe_sysctl_req_deps_missing)"
fi
printf "  %-28s %s\n" "sysctl:" "$probe_sysctl_table_result"
if test "$probe_routingtable_req_deps_ok" = "yes"; then
  probe_routingtable_table_result="yes"
else
  probe_routingtable_table_result="NO (missing: $probe_routingtable_req_deps_missing)"
fi
printf "  %-28s %s\n" "routingtable:" "$probe_routingtable_table_result"
if test "$probe_gconf_req_deps_ok" = "yes"; then
  probe_gconf_table_result="yes"
else
  probe_gconf_table_result="NO (missing: $probe_gconf_req_deps_missing)"
fi
printf "  %-28s %s\n" "gconf:" "$probe_gconf_table_result"
if test "$probe_isainfo_req_deps_ok" = "yes"; then
  probe_isainfo_table_result="yes"
else
  probe_isainfo_table_result="NO (missing: $probe_isainfo_req_deps_missing)"
fi
printf "  %-28s %s\n" "isainfo:" "$probe_isainfo_table_result"
if test "$probe_package_req_deps_ok" = "yes"; then
  probe_package_table_result="yes"
else
  probe_package_table_result="NO (missing: $probe_package_req_deps_missing)"
fi
printf "  %-28s %s\n" "package:" "$probe_package_table_result"
if test "$probe_patch_req_deps_ok" = "yes"; then
  probe_patch_table_result="yes"
else
  probe_patch_table_result="NO (missing: $probe_patch_req_deps_missing)"
fi
printf "  %-28s %s\n" "patch:" "$probe_patch_table_result"
if test "$probe_smf_req_deps_ok" = "yes"; then
  probe_smf_table_result="yes"
else
  probe_smf_table_result="NO (missing: $probe_smf_req_deps_missing)"
fi
printf "  %-28s %s\n" "smf:" "$probe_smf_table_result"
if test "$probe_partition_req_deps_ok" = "yes"; then
  probe_partition_table_result="yes"
else
  probe_partition_table_result="NO (missing: $probe_partition_req_deps_missing)"
fi
printf "  %-28s %s\n" "partition:" "$probe_partition_table_result"
if test "$probe_inetlisteningservers_req_deps_ok" = "yes"; then
  probe_inetlisteningservers_table_result="yes"
else
  probe_inetlisteningservers_table_result="NO (missing: $probe_inetlisteningservers_req_deps_missing)"
fi
printf "  %-28s %s\n" "inetlisteningservers:" "$probe_inetlisteningservers_table_result"
if test "$probe_iflisteners_req_deps_ok" = "yes"; then
  probe_iflisteners_table_result="yes"
else
  probe_iflisteners_table_result="NO (missing: $probe_iflisteners_req_deps_missing)"
fi
printf "  %-28s %s\n" "iflisteners:" "$probe_iflisteners_table_result"
if test "$probe_selinuxboolean_req_deps_ok" = "yes"; then
  probe_selinuxboolean_table_result="yes"
else
  probe_selinuxboolean_table_result="NO (missing: $probe_selinuxboolean_req_deps_missing)"
fi
printf "  %-28s %s\n" "selinuxboolean:" "$probe_selinuxboolean_table_result"
if test "$probe_selinuxsecuritycontext_req_deps_ok" = "yes"; then
  probe_selinuxsecuritycontext_table_result="yes"
else
  probe_selinuxsecuritycontext_table_result="NO (missing: $probe_selinuxsecuritycontext_req_deps_missing)"
fi
printf "  %-28s %s\n" "selinuxsecuritycontext:" "$probe_selinuxsecuritycontext_table_result"
if test "$probe_rpminfo_req_deps_ok" = "yes"; then
  probe_rpminfo_table_result="yes"
else
  probe_rpminfo_table_result="NO (missing: $probe_rpminfo_req_deps_missing)"
fi
printf "  %-28s %s\n" "rpminfo:" "$probe_rpminfo_table_result"
if test "$probe_rpmverify_req_deps_ok" = "yes"; then
  probe_rpmverify_table_result="yes"
else
  probe_rpmverify_table_result="NO (missing: $probe_rpmverify_req_deps_missing)"
fi
printf "  %-28s %s\n" "rpmverify:" "$probe_rpmverify_table_result"
if test "$probe_rpmverifyfile_req_deps_ok" = "yes"; then
  probe_rpmverifyfile_table_result="yes"
else
  probe_rpmverifyfile_table_result="NO (missing: $probe_rpmverifyfile_req_deps_missing)"
fi
printf "  %-28s %s\n" "rpmverifyfile:" "$probe_rpmverifyfile_table_result"
if test "$probe_rpmverifypackage_req_deps_ok" = "yes"; then
  probe_rpmverifypackage_table_result="yes"
else
  probe_rpmverifypackage_table_result="NO (missing: $probe_rpmverifypackage_req_deps_missing)"
fi
printf "  %-28s %s\n" "rpmverifypackage:" "$probe_rpmverifypackage_table_result"
if test "$probe_dpkginfo_req_deps_ok" = "yes"; then
  probe_dpkginfo_table_result="yes"
else
  probe_dpkginfo_table_result="NO (missing: $probe_dpkginfo_req_deps_missing)"
fi
printf "  %-28s %s\n" "dpkginfo:" "$probe_dpkginfo_table_result"
if test "$probe_systemdunitproperty_req_deps_ok" = "yes"; then
  probe_systemdunitproperty_table_result="yes"
else
  probe_systemdunitproperty_table_result="NO (missing: $probe_systemdunitproperty_req_deps_missing)"
fi
printf "  %-28s %s\n" "systemdunitproperty:" "$probe_systemdunitproperty_table_result"
if test "$probe_systemdunitdependency_req_deps_ok" = "yes"; then
  probe_systemdunitdependency_table_result="yes"
else
  probe_systemdunitdependency_table_result="NO (missing: $probe_systemdunitdependency_req_deps_missing)"
fi
printf "  %-28s %s\n" "systemdunitdependency:" "$probe_systemdunitdependency_table_result"
echo
echo "  === configuration ==="
echo "  probe directory set to:      $probe_dir"
echo ""

echo "  === crypto === "
echo "  library:                     $crapi_libname"
echo "     libs:                     $crapi_LIBS"
echo "   cflags:                     $crapi_CFLAGS"
echo ""

echo "Valgrind checks enabled:       $vgcheck"
echo "CFLAGS:                        $CFLAGS"
echo "CXXFLAGS:                      $CXXFLAGS"
