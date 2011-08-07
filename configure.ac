#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([openscap], [0.7.4], [open-scap-list@redhat.com])
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
LT_CURRENT=0

## increment any time the source changes; set to if you increment CURRENT
LT_REVISION=1

## increment if any interfaces have been added; set to 0
## if any interfaces have been changed or removed. removal has
## precedence over adding, so set to 0 if both happened.
LT_AGE=0

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_DEFUN([canonical_wrap], [AC_REQUIRE([AC_CANONICAL_HOST])])
canonical_wrap

# Compiler flags
CFLAGS="$CFLAGS -pipe -std=c99 -W -Wall -Wnonnull -Wshadow -Wformat -Wundef -Wno-unused-parameter -Wmissing-prototypes -Wno-unknown-pragmas -D_GNU_SOURCE -DOSCAP_THREAD_SAFE -D_POSIX_C_SOURCE=200112L"

AS_CASE([$host],
[*solaris*], [CFLAGS="$CFLAGS -D__EXTENSIONS__"])

CFLAGS_OPTIMIZED="-O2 -finline-functions"
CFLAGS_DEBUGGING="-fno-inline-functions -O0 -g3"
CFLAGS_NODEBUG="-DNDEBUG -Wno-unused-function"


#
# fts_open
#
AC_CHECK_FUNCS([fts_open])

#
# sigwaitinfo
#
AC_CHECK_FUNC(sigwaitinfo, [sigwaitinfo_libs=""], [sigwaitinfo_libs="-lrt"])

#
# posix_memalign
#
AC_CHECK_FUNCS([posix_memalign memalign])

#
# xml2-config
#
AC_CHECK_PROG(
  [HAVE_LIBXML_CONFIG],
  [xml2-config],
  [yes],,,
)
if test "x${HAVE_LIBXML_CONFIG}" != "xyes"; then
  AC_MSG_FAILURE([xml2-config not found in PATH])
fi

#
# xslt-config
#
AC_CHECK_PROG(
  [HAVE_LIBXSLT_CONFIG],
  [xslt-config],
  [yes],,,
)
if test "x${HAVE_LIBXSLT_CONFIG}" != "xyes"; then
  AC_MSG_FAILURE([xslt-config not found in PATH])
fi


#
# pcre-config
#
AC_CHECK_PROG(
  [HAVE_PCRE_CONFIG],
  [pcre-config],
  [yes],,,
)
if test "x${HAVE_PCRE_CONFIG}" != "xyes"; then
  AC_MSG_FAILURE([pcre-config not found in PATH])
fi

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

# Checks for libraries.

#
# libxml2
#
AC_CHECK_LIB(
  [xml2],
  [xmlParseFile],
  [libxml_cflags=`xml2-config --cflags`
   libxml_libs=`xml2-config --libs`
  ],
  [AC_MSG_FAILURE([libxml2 library is missing])],
  [`xml2-config --libs`]
)

#
# libxslt
#
AC_CHECK_LIB(
  [xslt],
  [xsltApplyStylesheet],
  [libxslt_cflags=`xslt-config --cflags`
   libxslt_libs=`xslt-config --libs`
  ],
  [AC_MSG_FAILURE([libxslt library is missing])],
  [`xslt-config --libs`]
)

#
# libpcre
#
AC_CHECK_LIB(
  [pcre],
  [pcre_compile],
  [pcre_cflags=`pcre-config --cflags`
   pcre_libs=`pcre-config --libs`
  ],
  [AC_MSG_FAILURE([pcre library is missing])],
  [`pcre-config --libs`]
)

probe_independent_ldap57=no

AC_CHECK_LIB(
  [ldap],
  [ldap_initialize],
  [libopenldap_libs="-lldap -llber"
   libopenldap_cflags=""
   probe_independent_ldap57=yes
  ],
  [probe_independent_ldap57=no],
  []
)

crapi_cflags=""
crapi_libs=""

if test "${with_crypto}" = ""; then
   with_crypto=gcrypt
fi

case "${with_crypto}" in
     nss3)
        AC_CHECK_LIB([nss3], [NSS_NoDB_Init],
                     [crapi_cflags=`nss-config --cflags`
                      crapi_libs=`nss-config --libs`
                      crapi_libname="NSS 3.x"
                      AC_DEFINE([HAVE_NSS3], [1], [Define to 1 if you have 'NSS' library.])
                      AC_CHECK_LIB([nspr4], [PR_CreateThread],
                       	           [crapi_cflags="$crapi_cflags `nspr-config --cflags`"
                                    crapi_libs="$crapi_libs `nspr-config --libs`"
         	                    AC_DEFINE([HAVE_NSPR4], [1], [Define to 1 if you have 'NSPR' library.])],
                                   [AC_MSG_ERROR([library 'nspr4' is required for NSS.])],
                                   [`nspr-config --libs`])],
                     [AC_MSG_ERROR([library 'nss3' is required for NSS.])],
                     [`nss-config --libs`])
        ;;
    gcrypt)
         AC_CHECK_LIB([gcrypt], [gcry_check_version],
                      [crapi_cflags=`libgcrypt-config --cflags`
                       crapi_libs=`libgcrypt-config --libs`
                       crapi_libname="GCrypt"
                       AC_DEFINE([HAVE_GCRYPT], [1], [Define to 1 if you have 'gcrypt' library.])],
                      [AC_MSG_ERROR([library 'gcrypt' is required for GCrypt.])],
                      [`libgcrypt-config --libs`])
        ;;
         *)
          AC_MSG_ERROR([unknown crypto backend])
        ;;
esac

AC_SUBST(crapi_cflags)
AC_SUBST(crapi_libs)

case "${host}" in
    *-*-linux*)

#
# rpmlib
# Check for rpmlib version and provide RHEL5 compatibility functions when necessary
#
have_librpm=no
AC_SEARCH_LIBS(
  [rpmtsCreate],
  [rpm],
  [AC_CHECK_LIB(
     [rpm],
     [headerFormat],
     [rpm_cflags=`pkg-config rpm --cflags`
      rpm_libs=`pkg-config rpm --libs`
      have_librpm=yes
      AC_DEFINE([HAVE_LIBRPM47],[1],[Use RPM lib 4.7 compatibility])],
     [AC_MSG_CHECKING([for librpm v4.4])
      saved_ldflags=$LDFLAGS
      LDFLAGS="$LDFLAGS -lrpm -lrpmio"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
         [[#include <rpm/header.h>]],
         [[headerSprintf(0,0,0,0,0);]])],
        [rpm_cflags="-I/usr/include/rpm"
         rpm_libs="-lrpm -lrpmio"
         have_librpm=yes
         AC_DEFINE([HAVE_LIBRPM44],[1],[Use RPM lib 4.4 compatibility])
         AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no])
         AC_MSG_NOTICE([RPM missing headerFormat & headerSprintf.  RPM probes will not be enabled.  Install RPM 4.7 or 4.4 libraries to enabled RPM support.])]
      )
      LDFLAGS=$saved_ldflags
     ]
   )
  ],[AC_MSG_NOTICE([RPM missing rpmtsCreate compatibility.  RPM probes will not be enabled.])]
)
;;
esac

have_libapt_pkg=no
AC_CHECK_LIB(
  [apt-pkg],
  [pkgVersion],
  [deb_cflags=""
   deb_libs="-lapt-pkg"
   have_libapt_pkg=yes
  ]
)

#
# threads
#
pthread_cflags=error
pthread_ldflags=error

if test "x$pthread_ldflags" = "xerror"; then
   AC_CHECK_LIB(c_r, pthread_attr_init, [
                     pthread_cflags="-DOSCAP_THREAD_SAFE -D_THREAD_SAFE -pthread"
                     pthread_ldflags="-pthread" ])
fi

if test "x$pthread_ldflags" = "xerror"; then
   AC_CHECK_LIB(pthread, pthread_attr_init, [
                         pthread_cflags="-DOSCAP_THREAD_SAFE -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS"
                         pthread_ldflags="-lpthread" ])
fi

if test "x$pthread_ldflags" = "xerror"; then
   AC_CHECK_LIB(pthreads, pthread_attr_init, [
                          pthread_cflags="-DOSCAP_THREAD_SAFE -D_THREAD_SAFE"
                          pthread_ldflags="-lpthreads" ])
fi

if test "x$pthread_ldflags" = "xerror"; then
   AC_MSG_FAILURE(pthread library not found)
fi

AC_SUBST(pthread_cflags)
AC_SUBST(pthread_ldflags)

#
# libacl
#
AC_CHECK_LIB(
  [acl],
  [acl_extended_file],
  [libacl_cflags=
   libacl_libs=-lacl
   AC_DEFINE([HAVE_LIBACL], [1], [libacl is available])
  ]
)

#
# libblkid
#
AC_CHECK_LIB(
  [blkid],
  [blkid_get_tag_value],
  [libblkid_cflags=
   libblkid_libs=-lblkid
   AC_DEFINE([HAVE_BLKID_GET_TAG_VALUE], [1], [blkid_get_tag_value is available])
  ]
)

#
# selinux
#
have_selinux=no
AC_CHECK_LIB(
  [selinux],
  [is_selinux_enabled],
  [selinux_cflags=`pkg-config libselinux --cflags`
   selinux_libs=`pkg-config libselinux --libs`
   have_selinux=yes
   AC_DEFINE([ENABLE_PROBES_SELINUX], [1], [Define if SELinux is available])
  ]
)

# Checks for header files.

AC_HEADER_STDC
AM_CHECK_PYTHON_HEADERS
AC_CHECK_HEADERS(
  [pthread.h],,
  [AC_MSG_FAILURE([pthread header file is missing])]
)
AC_CHECK_HEADERS([endian.h sys/endian.h stdio_ext.h shadow.h],,,)

#
# linux/magic.h
#
AC_CHECK_HEADERS(
  [linux/magic.h],[AC_DEFINE([HAVE_PROC_MAGIC], [1], [linux/magic.h is avaialble])],
)


#
# xml
#
CPPFLAGS_SAVE="$CPPFLAGS"
CPPFLAGS="$libxml_cflags $CPPFLAGS"
AC_CHECK_HEADERS(
  [libxml2/libxml/xmlreader.h],,
  [AC_MSG_FAILURE([libxml2 header files are missing])]
)
CPPFLAGS="$CPPFLAGS_SAVE"

#
# xslt
#
CPPFLAGS_SAVE="$CPPFLAGS"
CPPFLAGS="$libxslt_cflags $CPPFLAGS"
AC_CHECK_HEADERS(
  [libxslt/xslt.h],,
  [AC_MSG_FAILURE([libxslt header files are missing])]
)
CPPFLAGS="$CPPFLAGS_SAVE"

#
# pcre.h
#
CPPFLAGS_SAVE="$CPPFLAGS"
CPPFLAGS="$pcre_cflags"
AC_CHECK_HEADERS(
  [pcre.h],,
  [AC_MSG_FAILURE([pcre header file is missing])]
)
CPPFLAGS="$CPPFLAGS_SAVE"

AC_PATH_PROG(PERL, perl)
PERL_INCLUDES="`$PERL -e 'use Config; print $Config{archlib}'`/CORE"
vendorlib="$(  $PERL -e 'use Config; print $Config{vendorlib}'  | sed "s|$($PERL -e 'use Config; print $Config{prefix}')||" )"
vendorarch="$( $PERL -e 'use Config; print $Config{vendorarch}' | sed "s|$($PERL -e 'use Config; print $Config{prefix}')||" )"
AC_SUBST([PERL_INCLUDES], ["$PERL_INCLUDES"])
AC_SUBST([perl_vendorlibdir], ['${prefix}'$vendorlib])
AC_SUBST([perl_vendorarchdir], ['${prefix}'$vendorarch])

AC_SUBST(sigwaitinfo_libs)

AC_SUBST(libxml_cflags)
AC_SUBST(libxml_libs)

AC_SUBST(libxslt_cflags)
AC_SUBST(libxslt_libs)

AC_SUBST(pcre_cflags)
AC_SUBST(pcre_libs)

AC_SUBST(libopenldap_cflags)
AC_SUBST(libopenldap_libs)

AC_SUBST(rpm_cflags)
AC_SUBST(rpm_libs)

AC_SUBST(deb_cflags)
AC_SUBST(deb_libs)

AC_SUBST(libacl_cflags)
AC_SUBST(libacl_libs)

AC_SUBST(libblkid_cflags)
AC_SUBST(libblkid_libs)

AC_SUBST(selinux_cflags)
AC_SUBST(selinux_libs)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strcasecmp strdup])




AC_ARG_ENABLE([oval],
     [AC_HELP_STRING([--enable-oval], [include support for OVAL (default=yes)])],
     [case "${enableval}" in
       yes) oval=yes ;;
       no)  oval=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-oval]) ;;
     esac],[oval=yes])

AC_ARG_ENABLE([probes],
     [AC_HELP_STRING([--enable-probes], [enable compilation of probes (default=yes)])],
     [case "${enableval}" in
       yes) probes=yes ;;
       no)  probes=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes]) ;;
     esac],[probes=yes])

if test "x${oval}" = xno; then
	probes=no
fi

if test "x${probes}" = xyes; then
	AC_DEFINE([ENABLE_PROBES], [1], [compilation of probes is enabled])

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

AC_ARG_ENABLE([cvss],
     [AC_HELP_STRING([--enable-cvss], [include support for CVSS (default=yes)])],
     [case "${enableval}" in
       yes) cvss=yes ;;
       no)  cvss=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cvss]) ;;
     esac],[cvss=yes])

AC_ARG_ENABLE([cve],
     [AC_HELP_STRING([--enable-cve], [include support for CVE (default=yes)])],
     [case "${enableval}" in
       yes) cve=yes ;;
       no)  cve=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cve]) ;;
     esac],[cve=yes])

AC_ARG_ENABLE([cpe],
     [AC_HELP_STRING([--enable-cpe], [include support for CPE (default=yes)])],
     [case "${enableval}" in
       yes) cpe=yes ;;
       no)  cpe=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpe]) ;;
     esac],[cpe=yes])

AC_ARG_ENABLE([cce],
     [AC_HELP_STRING([--enable-cce], [include support for CCE (default=yes)])],
     [case "${enableval}" in
       yes) cce=yes ;;
       no)  cce=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cce]) ;;
     esac],[cce=yes])

AC_ARG_ENABLE([xccdf],
     [AC_HELP_STRING([--enable-xccdf], [include support for XCCDF (default=yes)])],
     [case "${enableval}" in
       yes) xccdf=yes ;;
       no)  xccdf=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-xccdf]) ;;
     esac],[xccdf=yes])

AC_ARG_ENABLE([bindings],
     [AC_HELP_STRING([--enable-bindings], [enable compilation of bindings (default=yes)])],
     [case "${enableval}" in
       yes) bindings=yes ;;
       no)  bindings=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-bindings]) ;;
     esac],[bindings=yes])

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

if test "$regex_posix" = "yes"; then
   AC_DEFINE([USE_REGEX_POSIX], [1], [Use POSIX regular expressions])
else
   AC_DEFINE([USE_REGEX_PCRE], [1], [Use PCRE])
fi

if test "$ssp" = "yes"; then
   GCC_STACK_PROTECT_CC
   GCC_STACK_PROTECT_CXX
fi

if test "$probes_independent" = "yes"; then
#
# OpenDBX
#
AC_CHECK_LIB(
  [opendbx],
  [odbx_init],
  [libodbx_cflags=`pkg-config opendbx --cflags`
   libodbx_libs=`pkg-config opendbx --libs`
   have_odbx=yes
  ],
  [AC_MSG_NOTICE([OpenDBX library is missing])],
  [`pkg-config opendbx --libs`]
)
AC_SUBST(libodbx_cflags)
AC_SUBST(libodbx_libs)
fi

sql_probe_enabled=no

if test "$probes_independent" = "yes"; then
   if test "$have_odbx" = "yes"; then
      sql_probe_enabled=yes
   fi
fi

if test "$debug" = "yes"; then
   CFLAGS="$CFLAGS $CFLAGS_DEBUGGING"
else
   CFLAGS="$CFLAGS $CFLAGS_NODEBUG"
   AC_SUBST([NODEBUG], [-DNDEBUG]) #for swig alloc.h
fi

AC_ARG_ENABLE([util-oscap],
     [AC_HELP_STRING([--enable-util-oscap], [enable compilation of the oscap utility (default=yes)])],
     [case "${enableval}" in
       yes) util_oscap=yes ;;
       no)  util_oscap=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-util-oscap]) ;;
     esac],[util_oscap=yes])

if test "$vgdebug" = "yes"; then
 if test "$HAVE_VALGRIND" = "yes"; then
   vgcheck="yes"
 else
   vgcheck="valgrind not installed"
 fi
else
   vgcheck="no"
fi

AM_CONDITIONAL([WANT_OVAL], test "$oval" = yes)
AM_CONDITIONAL([WANT_CVSS], test "$cvss" = yes)
AM_CONDITIONAL([WANT_CVE],  test "$cve"  = yes)
AM_CONDITIONAL([WANT_CPE],  test "$cpe"  = yes)
AM_CONDITIONAL([WANT_CCE],  test "$cce"  = yes)
AM_CONDITIONAL([WANT_XCCDF],  test "$xccdf"  = yes)
AM_CONDITIONAL([WANT_PROBES], test "$probes" = yes)
AM_CONDITIONAL([WANT_PROBES_INDEPENDENT], test "$probes_independent" = yes)
AM_CONDITIONAL([WANT_PROBES_UNIX], test "$probes_unix" = yes)
AM_CONDITIONAL([WANT_PROBES_LINUX], test "$probes_linux" = yes)
AM_CONDITIONAL([WANT_PROBES_SOLARIS], test "$probes_solaris" = yes)
AM_CONDITIONAL([WANT_UTIL_OSCAP], test "$util_oscap" = yes)
AM_CONDITIONAL([ENABLE_PROBE_RPMINFO], test "$have_librpm" = yes)
AM_CONDITIONAL([ENABLE_PROBE_RPMINFO44], test "$have_librpm44" = yes)
AM_CONDITIONAL([ENABLE_PROBE_DPKGINFO], test "$have_libapt_pkg" = yes)
AM_CONDITIONAL([WANT_BINDINGS], test "$bindings" = yes)
AM_CONDITIONAL([ENABLE_VALGRIND_TESTS], test "$vgcheck" = yes)
AM_CONDITIONAL([HAVE_ODBX], test "$have_odbx" = yes)
AM_CONDITIONAL([ENABLE_PROBE_LDAP57], test "$probe_independent_ldap57" = yes)
AM_CONDITIONAL(ENABLE_PROBES_SELINUX, [test "$have_selinux" = "yes"])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 swig/Makefile
                 src/OVAL/Makefile
                 src/OVAL/probes/Makefile
                 src/OVAL/probes/probe/Makefile
                 src/OVAL/probes/crapi/Makefile
                 src/OVAL/probes/SEAP/Makefile
                 src/OVAL/probes/SEAP/generic/rbt/Makefile
                 src/CVSS/Makefile
                 src/CVE/Makefile
                 src/CPE/Makefile
                 src/CCE/Makefile
                 src/XCCDF/Makefile
                 src/XCCDF_POLICY/Makefile
                 src/common/Makefile
                 tests/Makefile
                 tests/API/Makefile
                 tests/API/CCE/Makefile
                 tests/API/CVE/Makefile
                 tests/API/CPE/Makefile
                 tests/API/CPE/uri/Makefile
                 tests/API/CPE/lang/Makefile
                 tests/API/CPE/dict/Makefile
                 tests/API/CVSS/Makefile
                 tests/API/SEAP/Makefile
                 tests/API/OVAL/Makefile
                 tests/API/XCCDF/Makefile
                 tests/API/crypt/Makefile
                 tests/API/probes/Makefile
                 tests/probes/Makefile
                 tests/probes/file/Makefile
                 tests/probes/uname/Makefile
                 tests/probes/shadow/Makefile
                 tests/probes/family/Makefile
                 tests/probes/vgtest/Makefile
                 tests/probes/process/Makefile
                 tests/probes/sysinfo/Makefile
                 tests/probes/rpminfo/Makefile
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
                 tests/mitre/Makefile
                 utils/Makefile
                 xsl/Makefile
                 schemas/Makefile
		 libopenscap.pc])

AC_OUTPUT

echo "******************************************************"
echo "OpenSCAP will be compiled with the following settings:"
echo
echo "CCE enabled:                   $cce"
echo "CPE enabled:                   $cpe"
echo "CVE enabled:                   $cve"
echo "CVSS enabled:                  $cvss"
echo "OVAL enabled:                  $oval"
echo "OVAL probes enabled:           $probes"

if test "x$probes" = "xyes"; then
echo "  system info probe:           $probes"
echo ""
echo "  === independent probes ==="
echo "  family probe:                $probes_independent"
echo "  filehash probe:              $probes_independent"
echo "  filehash58 probe:            $probes_independent"
echo "  textfilecontent probe:       $probes_independent"
echo "  textfilecontent54 probe:     $probes_independent"
echo "  sql probe                    $sql_probe_enabled"
echo "  sql57 probe:                 $sql_probe_enabled"
echo "  xmlfilecontent probe:        $probes_independent"
echo "  ldap57 probe:                $probe_independent_ldap57"
echo ""

echo "  === unix probes ==="
echo "  dnscache probe:              $probes_unix"
echo "  file probe:                  $probes_unix"
echo "  interface probe:             $probes_unix"
echo "  password probe:              $probes_unix"
echo "  process probe:               $probes_unix"
echo "  runlevel probe:              $probes_unix"
echo "  shadow probe:                $probes_unix"
echo "  uname probe:                 $probes_unix"
echo "  xinetd probe:                $probes_unix"
echo ""

echo "  === linux probes ==="
echo "  dpkginfo probe:              $have_libapt_pkg"
echo "  inetlisteningservers probe:  $probes_linux"
echo "  iflisteners probe:           $probes_linux"
echo "  rpminfo probe:               $have_librpm"
echo "  rpmverify probe:             $have_librpm"
echo "  partition probe:             $probes_linux"
echo "  selinuxboolean probe:        $have_selinux"
echo "  selinuxsecuritycontext probe: $have_selinux"
echo ""

echo "  === solaris probes ==="
echo "  isainfo probe:               $probes_solaris"
echo "  package probe:               $probes_solaris"
echo "  package54 probe:             $probes_solaris"
echo "  patch probe:                 $probes_solaris"
echo "  smf probe:                   $probes_solaris"
echo ""

echo "  === configuration ==="
echo "  probe directory set to:      $probe_dir"
echo ""
fi

echo "  === crypto === "
echo "  library:                     $crapi_libname"
echo "  libs:                        $crapi_libs"
echo "  cflags:                      $crapi_cflags"
echo ""

echo "XCCDF enabled:                 $xccdf"
echo "python bindings enabled:       $bindings"
echo "oscap tool:                    $util_oscap"
echo "use POSIX regex:               $regex_posix"
echo "debugging flags enabled:       $debug"
echo "Valgrind checks enabled:       $vgcheck"
echo "CFLAGS:                        $CFLAGS"
echo "CXXFLAGS:                      $CXXFLAGS"
