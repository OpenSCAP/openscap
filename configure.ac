#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([openscap], [0.1.2], [open-scap-list@redhat.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE
AM_PROG_LIBTOOL
AM_PATH_PYTHON

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# swig
AC_PROG_SWIG([1.3.30])

# Compiler flags
CFLAGS="$CFLAGS -W -Wall -Wshadow -Wformat -Wundef"

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

# Requires automake >= 1.10.2
#
#AM_COND_IF([WANT_OVAL],
#           [AC_CONFIG_FILES([src/OVAL/Makefile
#                             examples/OVAL/Makefile])])
#AM_COND_IF([WANT_CVSS],
#           [AC_CONFIG_FILES([src/CVSS/Makefile
#                             examples/CVSS/Makefile])])
#AM_COND_IF([WANT_CVE],
#           [AC_CONFIG_FILES([src/CVE/Makefile
#                             examples/CVE/Makefile])])
#AM_COND_IF([WANT_CPE],
#           [AC_CONFIG_FILES([src/CPE/Makefile
#                             examples/CPE/Makefile])])
#AM_COND_IF([WANT_CCE],
#           [AC_CONFIG_FILES([src/CCE/Makefile
#                             examples/CCE/Makefile])])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 swig/Makefile
                 src/OVAL/Makefile
                 src/CVSS/Makefile
                 src/CVE/Makefile
                 src/CPE/Makefile
                 src/CCE/Makefile])

#AC_CONFIG_FILES([Makefile
#                 src/Makefile
#                 examples/Makefile
#                 swig/Makefile
#                 src/OVAL/Makefile
#                 examples/OVAL/Makefile
#                 src/CVSS/Makefile
#                 examples/CVSS/Makefile
#                 src/CVE/Makefile
#                 examples/CVE/Makefile
#                 src/CPE/Makefile
#                 examples/CPE/Makefile
#                 src/CCE/Makefile
#                 examples/CCE/Makefile])


AC_OUTPUT
