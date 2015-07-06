# ! MAKE SURE YOU ARE EDITING THE ac_probes/configure.ac.tpl FILE,
# ! THE configure.ac FILE ITSELF IS GENERATED FROM THE TEMPLATE USING
# ! ac_probes/ac_probes.sh

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([openscap], [1.3.0], [open-scap-list@redhat.com])
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
LT_CURRENT=13

## increment any time the source changes; set 0 to if you increment CURRENT
LT_REVISION=1

## increment if any interfaces have been added; set to 0
## if any interfaces have been changed or removed. removal has
## precedence over adding, so set to 0 if both happened.
LT_AGE=5

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

AC_ARG_ENABLE([cce],
     [AC_HELP_STRING([--enable-cce], [include support for CCE (default=no)])],
     [case "${enableval}" in
       yes) cce=yes ;;
       no)  cce=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cce]) ;;
     esac],[cce=no])

AC_ARG_ENABLE([python],
     [AC_HELP_STRING([--enable-python], [enable compilation of python bindings (default=yes)])],
     [case "${enableval}" in
       yes) python_bind=yes ;;
       no)  python_bind=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-python]) ;;
     esac],[python_bind=yes])

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

AC_ARG_ENABLE([probes],
     [AC_HELP_STRING([--enable-probes], [enable compilation of probes (default=yes)])],
     [case "${enableval}" in
       yes) probes=yes ;;
       no)  probes=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes]) ;;
     esac],[probes=yes])

AC_CHECK_HEADERS([syslog.h ftw.h])
AC_CHECK_FUNCS([posix_memalign memalign])

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
echo
echo '* Checking for bz2 library (optional dependency of libopenscap)'
AC_CHECK_LIB([bz2], [BZ2_bzReadOpen],
	[
	        AC_DEFINE([HAVE_BZ2], [1], [Define to 1 if there is libbz2 available.])
	        LIBS="$LIBS -lbz2"
		AC_CHECK_PROG([HAVE_BZIP2],[bzip2],[yes],,,)
	],[
	        AC_MSG_NOTICE([!!! libbz2 not found. Bzip2 support will be disabled !!!])
	])
AM_CONDITIONAL([HAVE_BZIP2], [test "x${HAVE_BZIP2}" = xyes])

@@@@PROBE_HEADERS@@@@

@@@@PROBE_LIBRARIES@@@@

if test "$probes" = "yes"; then
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
fi # probes = yes

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

if test "$probes" = "yes"; then
   AC_DEFINE([OVAL_PROBES_ENABLED], [1], [Compile the probe subsystem])

   if test "x${libexecdir}" = xNONE; then
      probe_dir="/usr/local/libexec/openscap"
   else
      EXPAND_DIR(probe_dir,"${libexecdir}/openscap")
   fi
   AC_SUBST(probe_dir)
fi

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

AC_ARG_ENABLE([util-oscap-ssh],
     [AC_HELP_STRING([--enable-util-oscap-ssh], [enable compilation of the oscap-ssh utility (default=yes)])],
     [case "${enableval}" in
       yes) util_oscap_ssh=yes ;;
       no)  util_oscap_ssh=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-util-oscap-ssh]) ;;
     esac],[util_oscap_ssh=yes])

AC_ARG_ENABLE([util-oscap-docker],
     [AC_HELP_STRING([--enable-util-oscap-docker], [enable compilation of the oscap-docker utility (default=yes)])],
     [case "${enableval}" in
       yes) util_oscap_docker=yes ;;
       no)  util_oscap_docker=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-util-oscap-docker]) ;;
     esac],[util_oscap_docker=yes])

if test "$vgdebug" = "yes"; then
 if test "$HAVE_VALGRIND" = "yes"; then
   vgcheck="yes"
 else
   AC_MSG_ERROR([valgrind not installed])
 fi
else
   vgcheck="no"
fi
AC_SUBST([vgcheck])

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

@@@@PROBE_EVAL@@@@

AM_CONDITIONAL([WANT_CCE],  test "$cce"  = yes)
AM_CONDITIONAL([WANT_PROBES], test "$probes" = yes)
AM_CONDITIONAL([WANT_PROBES_INDEPENDENT], test "$probes_independent" = yes)
AM_CONDITIONAL([WANT_PROBES_UNIX], test "$probes_unix" = yes)
AM_CONDITIONAL([WANT_PROBES_LINUX], test "$probes_linux" = yes)
AM_CONDITIONAL([WANT_PROBES_SOLARIS], test "$probes_solaris" = yes)

AM_CONDITIONAL([WANT_SCE], test "$sce" = yes)
AM_CONDITIONAL([WANT_UTIL_OSCAP], test "$util_oscap" = yes)
AM_CONDITIONAL([WANT_UTIL_SCAP_AS_RPM], test "$util_scap_as_rpm" = yes)
AM_CONDITIONAL([WANT_UTIL_OSCAP_SSH], test "$util_oscap_ssh" = yes)
AM_CONDITIONAL([WANT_UTIL_OSCAP_DOCKER], test "$util_oscap_docker" = yes)
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
		src/source/Makefile
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
		tests/API/OVAL/glob_to_regex/Makefile
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
		tests/probes/symlink/Makefile
                 tests/probes/family/Makefile
                 tests/probes/process58/Makefile
                 tests/probes/sysinfo/Makefile
                 tests/probes/rpminfo/Makefile
                 tests/probes/rpmverifypackage/Makefile
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
		tests/bz2/Makefile
		tests/codestyle/Makefile
		tests/oval_details/Makefile

                 src/SCE/Makefile
                 tests/sce/Makefile])

AC_CONFIG_FILES([run],
                [chmod +x,-w run])
AC_CONFIG_FILES([tests/test_common.sh],
                [chmod +x,-w tests/test_common.sh])

AC_OUTPUT

echo "******************************************************"
echo "OpenSCAP will be compiled with the following settings:"
echo
echo "oscap tool:                    $util_oscap"
echo "scap-as-rpm tool:              $util_scap_as_rpm"
echo "oscap-ssh tool:                $util_oscap_ssh"
echo "oscap-docker tool:             $util_oscap_docker"
echo "python2 bindings enabled:      $python_bind"
echo "python3 bindings enabled:      $python3_bind"
echo "perl bindings enabled:         $perl_bind"
echo "use POSIX regex:               $regex_posix"
echo "SCE enabled                    $sce"
echo "debugging flags enabled:       $debug"
echo "CCE enabled:                   $cce"
echo

if test "$probes" = "yes"; then
@@@@PROBE_TABLE@@@@
echo
echo "  === configuration ==="
echo "  probe directory set to:      $probe_dir"
echo ""
fi # probes = "yes"
echo "  === crypto === "
echo "  library:                     $crapi_libname"
echo "     libs:                     $crapi_LIBS"
echo "   cflags:                     $crapi_CFLAGS"
echo ""

echo "Valgrind checks enabled:       $vgcheck"
echo "CFLAGS:                        $CFLAGS"
echo "CXXFLAGS:                      $CXXFLAGS"
