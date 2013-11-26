# strsep.m4 serial 10
dnl Copyright (C) 2002-2004, 2007, 2009-2013 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRSEP],
[
  dnl Persuade glibc <string.h> to declare strsep().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  dnl The strsep() declaration in lib/string.in.h uses 'restrict'.
  AC_REQUIRE([AC_C_RESTRICT])

  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_CHECK_FUNCS([strsep])
  if test $ac_cv_func_strsep = no; then
    HAVE_STRSEP=0
  fi
])

# Prerequisites of lib/strsep.c.
AC_DEFUN([gl_PREREQ_STRSEP], [:])
