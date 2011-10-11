#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([openscap], [0.8.0], [open-scap-list@redhat.com])
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
LT_CURRENT=1

## increment any time the source changes; set 0 to if you increment CURRENT
LT_REVISION=0

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

case $host in
  *solaris*) :
    CFLAGS="$CFLAGS -D__EXTENSIONS__" ;;
esac

CFLAGS_OPTIMIZED="-O2 -finline-functions"
CFLAGS_DEBUGGING="-fno-inline-functions -O0 -g3"
CFLAGS_NODEBUG="-DNDEBUG -Wno-unused-function"

@@@@PROBE_DECL@@@@

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
                     pthread_LIBS="-pthread" ])
fi

if test "x$pthread_LIBS" = "xerror"; then
   AC_CHECK_LIB(pthread, pthread_attr_init, [
                         pthread_CFLAGS="-DOSCAP_THREAD_SAFE -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS"
                         pthread_LIBS="-lpthread" ])
fi

if test "x$pthread_LIBS" = "xerror"; then
   AC_CHECK_LIB(pthreads, pthread_attr_init, [
                          pthread_CFLAGS="-DOSCAP_THREAD_SAFE -D_THREAD_SAFE"
                          pthread_LIBS="-lpthreads" ])
fi

if test "x$pthread_LIBS" = "xerror"; then
   AC_MSG_FAILURE(pthread library is missing)
fi

LIBS=$SAVE_LIBS
AC_SUBST(pthread_CFLAGS)
AC_SUBST(pthread_LIBS)

PKG_CHECK_MODULES([xml2], [libxml-2.0 >= 2.0],[],
			  AC_MSG_FAILURE([libxml-2.0 devel support is missing]))

PKG_CHECK_MODULES([xslt], [libxslt >= 1.1],[],
			  AC_MSG_FAILURE([libxslt devel support is missing]))

PKG_CHECK_MODULES([exslt], [libexslt >= 0.8],[],
			  AC_MSG_FAILURE([libexslt devel support is missing]))

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
	LIBS=$SAVE_LIBS
        AC_DEFINE([HAVE_GCRYPT], [1], [Define to 1 if you have 'gcrypt' library.])
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

@@@@PROBE_HEADERS@@@@

@@@@PROBE_LIBRARIES@@@@
echo

AC_PATH_PROG(PERL, perl)
PERL_INCLUDES="`$PERL -e 'use Config; print $Config{archlib}'`/CORE"
vendorlib="$(  $PERL -e 'use Config; print $Config{vendorlib}'  | sed "s|$($PERL -e 'use Config; print $Config{prefix}')||" )"
vendorarch="$( $PERL -e 'use Config; print $Config{vendorarch}' | sed "s|$($PERL -e 'use Config; print $Config{prefix}')||" )"
AC_SUBST([PERL_INCLUDES], ["$PERL_INCLUDES"])
AC_SUBST([perl_vendorlibdir], ['${prefix}'$vendorlib])
AC_SUBST([perl_vendorarchdir], ['${prefix}'$vendorarch])

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

@@@@PROBE_EVAL@@@@

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
AM_CONDITIONAL([WANT_BINDINGS], test "$bindings" = yes)
AM_CONDITIONAL([ENABLE_VALGRIND_TESTS], test "$vgcheck" = yes)

#
# Core
#
AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 xsl/Makefile
                 schemas/Makefile
		 libopenscap.pc
                 src/common/Makefile
                 tests/Makefile
                 tests/API/Makefile

		 swig/Makefile

		 utils/Makefile

		 src/OVAL/Makefile
                 tests/API/OVAL/Makefile
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
	   	 tests/probes/family/Makefile
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

		 tests/probes/vgtest/Makefile

           	 src/CVSS/Makefile
                 tests/API/CVSS/Makefile

           	 src/CVE/Makefile
                 tests/API/CVE/Makefile

		 src/CPE/Makefile
                 tests/API/CPE/Makefile
		 tests/API/CPE/uri/Makefile
                 tests/API/CPE/lang/Makefile
                 tests/API/CPE/dict/Makefile

           	 src/CCE/Makefile
                 tests/API/CCE/Makefile

           	 src/XCCDF/Makefile
	         src/XCCDF_POLICY/Makefile
                 tests/API/XCCDF/Makefile])

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
echo
@@@@PROBE_TABLE@@@@
echo
echo "  === configuration ==="
echo "  probe directory set to:      $probe_dir"
echo ""

echo "  === crypto === "
echo "  library:                     $crapi_libname"
echo "     libs:                     $crapi_LIBS"
echo "   cflags:                     $crapi_CFLAGS"
echo ""

echo "XCCDF enabled:                 $xccdf"
echo "python bindings enabled:       $bindings"
echo "oscap tool:                    $util_oscap"
echo "use POSIX regex:               $regex_posix"
echo "debugging flags enabled:       $debug"
echo "Valgrind checks enabled:       $vgcheck"
echo "CFLAGS:                        $CFLAGS"
echo "CXXFLAGS:                      $CXXFLAGS"
