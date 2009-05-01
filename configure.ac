#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([openscap], [0.3.3], [open-scap-list@redhat.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([foreign])
AC_DISABLE_STATIC
AM_PROG_LIBTOOL
AM_PATH_PYTHON

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# swig
AC_PROG_SWIG([1.3.29])

# Compiler flags
CFLAGS="$CFLAGS -W -Wall -Wshadow -Wformat -Wundef -D_GNU_SOURCE"

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

# Checks for libraries.

#
# libxml2
#
AC_CHECK_LIB(
  [xml2],
  [xmlSaveToBuffer],
  [libxml_cflags=`xml2-config --cflags`
   libxml_libs=`xml2-config --libs`
  ],
  [AC_MSG_FAILURE([libxml2 library is missing])],
  [`xml2-config --libs`]
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

# Checks for header files.

AC_HEADER_STDC
AM_CHECK_PYTHON_HEADERS

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


AC_SUBST(libxml_cflags)
AC_SUBST(libxml_libs)

AC_SUBST(pcre_cflags)
AC_SUBST(pcre_libs)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strcasecmp strdup])

AC_ARG_ENABLE([oval],
     [  --enable-oval           include support for OVAL [[default=yes]]],
     [case "${enableval}" in
       yes) oval=yes ;;
       no)  oval=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-oval]) ;;
     esac],[oval=yes])

AC_ARG_ENABLE([probes],
     [  --enable-probes         enable compilation of probes [[default=no]]],
     [case "${enableval}" in
       yes) probes=yes ;;
       no)  probes=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes]) ;;
     esac],[probes=no])

AC_ARG_ENABLE([probes-redhat],
     [  --enable-probes-redhat  enable compilation of probes for Fedora/RHEL OS [[default=no]]],
     [case "${enableval}" in
       yes) probes_rh=yes ;;
       no)  probes_rh=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-probes-redhat]) ;;
     esac],[probes_rh=no])
     
AC_ARG_ENABLE([cvss],
     [  --enable-cvss           include support for CVSS [[default=yes]]],
     [case "${enableval}" in
       yes) cvss=yes ;;
       no)  cvss=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cvss]) ;;
     esac],[cvss=yes])

AC_ARG_ENABLE([cve],
     [  --enable-cve            include support for CVE [[default=yes]]],
     [case "${enableval}" in
       yes) cve=yes ;;
       no)  cve=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cve]) ;;
     esac],[cve=yes])

AC_ARG_ENABLE([cpe],
     [  --enable-cpe            include support for CPE [[default=yes]]],
     [case "${enableval}" in
       yes) cpe=yes ;;
       no)  cpe=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpe]) ;;
     esac],[cpe=yes])

AC_ARG_ENABLE([cce],
     [  --enable-cce            include support for CCE [[default=yes]]],
     [case "${enableval}" in
       yes) cce=yes ;;
       no)  cce=no  ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-cce]) ;;
     esac],[cce=yes])

AM_CONDITIONAL([WANT_OVAL], test "$oval" = yes)
AM_CONDITIONAL([WANT_CVSS], test "$cvss" = yes)
AM_CONDITIONAL([WANT_CVE],  test "$cve"  = yes)
AM_CONDITIONAL([WANT_CPE],  test "$cpe"  = yes)
AM_CONDITIONAL([WANT_CCE],  test "$cce"  = yes)
AM_CONDITIONAL([WANT_PROBES], test "$probes" = yes)
AM_CONDITIONAL([WANT_PROBES_REDHAT], test "$probes_rh" = yes)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 swig/Makefile
                 src/OVAL/Makefile
                 src/OVAL/probes/Makefile
                 src/OVAL/probes/SEAP/Makefile
                 src/CVSS/Makefile
                 src/CVE/Makefile
                 src/CPE/Makefile
                 src/CCE/Makefile
                 swig/OVAL/Makefile
                 swig/CVSS/Makefile
                 swig/CVE/Makefile
                 swig/CPE/Makefile
                 swig/CCE/Makefile
                 examples/Makefile])

AC_OUTPUT
