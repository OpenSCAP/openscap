## ------------------------                                 -*- Autoconf -*-
## Python file handling
## From Andrew Dalke
## Updated by James Henstridge
## Refactored by Matej Tyc
## ------------------------
# Copyright (C) 1999-2018 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

m4_define([SETUP_SYSCONFIG],
[[
import sys
# Prefer sysconfig over distutils.sysconfig, for better compatibility
# with python 3.x.  See automake bug#10227.
try:
    import sysconfig
except ImportError:
    can_use_sysconfig = 0
else:
    can_use_sysconfig = 1
# Can't use sysconfig in CPython 2.7, since it's broken in virtualenvs:
# <https://github.com/pypa/virtualenv/issues/118>
try:
    from platform import python_implementation
    if python_implementation() == 'CPython' and sys.version_info[[:2]] == (2, 7):
        can_use_sysconfig = 0
except ImportError:
    pass
]])


dnl
dnl $1: The prefix
dnl $2: 0 for platform agnostic sitedir
dnl     1 for platform specific sitedir
m4_define([_GET_SITEDIR],
[[if can_use_sysconfig:
    sitedir = sysconfig.get_path('platlib', vars={'platbase':'$1'})
else:
    from distutils import sysconfig
    sitedir = sysconfig.get_python_lib($2, 0, prefix='$1')
sys.stdout.write(sitedir)
]])


m4_define([PY_SNIPPET_GET_SITEDIR_PLATFORM_AGNOSTIC],
	SETUP_SYSCONFIG[]_GET_SITEDIR([$1], 0))

m4_define([PY_SNIPPET_GET_SITEDIR_PLATFORM_SPECIFIC],
	SETUP_SYSCONFIG[]_GET_SITEDIR([$1], 1))


dnl
dnl $1: Major version that has to match
dnl $2: Return code if major version doesn't match
dnl $3: Minimal MAJOR.MINOR.PATCH version
dnl $4: Return code if there is no matching issue with major version, but minimal version requirements aren't met.
dnl
dnl This test uses sys.hexversion instead of the string equivalent (first
dnl word of sys.version), in order to cope with versions such as 2.2c1.
dnl This supports Python 2.0 or higher. (2.0 was released on October 16, 2000).
m4_define([PY_SNIPPET_CHECK_VERSION],
[[
import sys
# split strings by '.' and convert to numeric.  Append some zeros
# because we need at least 4 digits for the hex conversion.
# map returns an iterator in Python 3.0 and a list in 2.x
]m4_if([$1], , , [[# Check that the major Python version is OK
if sys.version_info[0] != int($1): sys.exit($2)]])[
minver = list(map(int, '$3'.split('.'))) + [0, 0, 0]
minverhex = 0
# xrange is not present in Python 3.0 and range returns an iterator
for i in list(range(0, 4)): minverhex = (minverhex << 8) + minver[i]
if sys.hexversion < minverhex: sys.exit($4)
]])

dnl
dnl Output space-separated list of Python interpreter names.
dnl The list is ordered, versionless name comes first, then comes name with major version
dnl and then name with minor versions. The ordering is descending - higher versions come first.
dnl $1: Major Python version (optional).
dnl     If a major Python version is selected, it is made sure that Python interpreters of
dnl     are sure to be of different major version are left out. However, the 'python' interpreter is
dnl     always kept, and its major version is unknown.
dnl FIXME: Remove the need to hard-code Python versions here.
m4_define([_AM_PYTHON_INTERPRETER_LIST],
[[python ]m4_if([$1], 2, , [[python3 ]])m4_if([$1], 3, , [[python2 ]])dnl
m4_if([$1], 2, , [[python3.6 python3.5 python3.4 python3.3 python3.2 python3.1 python3.0 ]])dnl
m4_if([$1], 3, , [[python2.7 python2.6 python2.5 python2.4 python2.3 python2.2 python2.1 python2.0]])])

# AM_PATH_PYTHON([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# AM_PATH_PYTHON_OF_MAJOR_VERSION([MAJOR_VERSION], [MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------------------------
# Adds support for distributing Python modules and packages.  To
# install modules, copy them to $(pythondir), using the python_PYTHON
# automake variable.  To install a package with the same name as the
# automake package, install to $(pkgpythondir), or use the
# pkgpython_PYTHON automake variable.
#
# The variables $(pyexecdir) and $(pkgpyexecdir) are provided as
# locations to install python extension modules (shared libraries).
# Another macro is required to find the appropriate flags to compile
# extension modules.
#
# If your package is configured with a different prefix to python,
# users will have to add the install directory to the PYTHONPATH
# environment variable, or create a .pth file (see the python
# documentation for details).
#
# If the MINIMUM-VERSION argument is passed, AM_PATH_PYTHON will
# cause an error if the version of python installed on the system
# doesn't meet the requirement.  MINIMUM-VERSION should consist of
# numbers and dots only.
AC_DEFUN([AM_PATH_PYTHON], [AM_PATH_PYTHON_OF_MAJOR_VERSION([], $@)])
AC_DEFUN([AM_PATH_PYTHON_OF_MAJOR_VERSION],
[
	dnl If major version is unknown, minimal minor version is 2.0
	m4_pushdef([MINIMAL_MINOR_VERSION], [m4_default([$2], m4_case([$1], 2, 2.0, 3, 3.0, 2.0))])
	dnl Find a Python interpreter.  Python versions prior to 2.0 are not
	dnl supported. (2.0 was released on October 16, 2000).

	AC_ARG_VAR([PYTHON$1], [the Python interpreter])

	dnl A version check is needed.
	AS_IF([test -n "$PYTHON$1"],
		[# If the user set $PYTHON$1, use it and don't search something else.
		AC_MSG_NOTICE([Caught the '\$PYTHON$1' variable containing '$PYTHON$1'])
		AC_MSG_CHECKING([whether $PYTHON$1 version is >= MINIMAL_MINOR_VERSION])
		AM_PYTHON_CHECK_VERSION([$PYTHON$1], [$1], MINIMAL_MINOR_VERSION,
			[AC_MSG_RESULT([yes])],
			[AC_MSG_RESULT([no])
			AC_MSG_ERROR([The '$PYTHON$1' Python interpreter supplied via the PYTHON$1 variable is not a python $1 interpreter.])],
			[AC_MSG_RESULT([no])
			AC_MSG_ERROR([The '$PYTHON$1' Python interpreter supplied via the PYTHON$1 variable is older than ]MINIMAL_MINOR_VERSION.)],
			[AC_MSG_RESULT([no])
			AC_MSG_ERROR([Unknown error while trying the '$PYTHON$1' Python interpreter. Does it exist?])])
		am_display_PYTHON$1=$PYTHON$1
], [
		# Otherwise, try each interpreter until we find one that satisfies VERSION.
		AC_CACHE_CHECK([for a Python$1 interpreter with version >= MINIMAL_MINOR_VERSION],
			[am_cv_pathless_PYTHON$1],[
			for am_cv_pathless_PYTHON$1 in _AM_PYTHON_INTERPRETER_LIST([$1]) none; do
				test "$am_cv_pathless_PYTHON$1" = none && break
				AM_PYTHON_CHECK_VERSION([$am_cv_pathless_PYTHON$1], [$1], MINIMAL_MINOR_VERSION,
					[echo "Tried Python interpreter '$am_cv_pathless_PYTHON$1': Success" >&AS_MESSAGE_LOG_FD
					break],
					[echo "Tried Python interpreter '$am_cv_pathless_PYTHON$1': Major version mismatch." >&AS_MESSAGE_LOG_FD],
					[echo "Tried Python interpreter '$am_cv_pathless_PYTHON$1': Too old." >&AS_MESSAGE_LOG_FD],
					[echo "Tried Python interpreter '$am_cv_pathless_PYTHON$1': Probably doesn't exist." >&AS_MESSAGE_LOG_FD])
			done])
		# Set $PYTHON$1 to the absolute path of $am_cv_pathless_PYTHON$1.
		AS_IF([test "$am_cv_pathless_PYTHON$1" = none],
			[PYTHON$1=:],
			[AC_PATH_PROG([PYTHON$1], [$am_cv_pathless_PYTHON$1])])
		am_display_PYTHON$1=$am_cv_pathless_PYTHON$1])

	dnl Run any user-specified action, or abort.
	AS_IF([test "$PYTHON$1" = :],
		[m4_default([$4], [AC_MSG_ERROR([no suitable Python$1 interpreter found])])
], [
		dnl Query Python for its major.minor version numbers.
		dnl Getting [:2] seems to be the best way to do this;
		dnl it's what "site.py" does in the standard library.
		AC_CACHE_CHECK([for $am_display_PYTHON$1 version], [am_cv_python$1_version],
			[[am_cv_python$1_version=`$PYTHON$1 -c "import sys; sys.stdout.write('%d.%d' % sys.version_info[:2])"`]])
		AC_SUBST([PYTHON$1_VERSION], [$am_cv_python$1_version])
		dnl Use the values of $prefix and $exec_prefix for the corresponding
		dnl values of PYTHON_PREFIX and PYTHON_EXEC_PREFIX.  These are made
		dnl distinct variables so they can be overridden if need be.  However,
		dnl general consensus is that you shouldn't need this ability.
		AC_SUBST([PYTHON$1_PREFIX], ['${prefix}'])
		AC_SUBST([PYTHON$1_EXEC_PREFIX], ['${exec_prefix}'])
		dnl At times (like when building shared libraries) you may want
		dnl to know which OS platform Python thinks this is.
		AC_CACHE_CHECK([for $am_display_PYTHON$1 platform], [am_cv_python$1_platform],
			[am_cv_python$1_platform=`$PYTHON$1 -c "import sys; sys.stdout.write(sys.platform)"`])
		AC_SUBST([PYTHON$1_PLATFORM], [$am_cv_python$1_platform])
		dnl Set up 4 directories:
		dnl pythondir -- where to install python scripts.  This is the
		dnl   site-packages directory, not the python standard library
		dnl   directory like in previous automake betas.  This behavior
		dnl   is more consistent with lispdir.m4 for example.
		dnl Query distutils for this directory.
		AC_CACHE_CHECK([for $am_display_PYTHON$1 script directory],
			[am_cv_python$1_pythondir],
			[AS_IF([test "x$prefix" = xNONE],
				[am_py_prefix=$ac_default_prefix],
				[am_py_prefix=$prefix])
			am_cv_python$1_pythondir=`$PYTHON$1 -c "PY_SNIPPET_GET_SITEDIR_PLATFORM_AGNOSTIC([$am_py_prefix])"`
			AS_CASE([$am_cv_python$1_pythondir],
				[$am_py_prefix*],
				[am__strip_prefix=`echo "$am_py_prefix" | sed 's|.|.|g'`
				am_cv_python$1_pythondir=`echo "$am_cv_python$1_pythondir" | sed "s,^$am__strip_prefix,$PYTHON$1_PREFIX,"`],
				[*],
				[AS_CASE([$am_py_prefix],
					[/usr|/System*],
					[],
					[*],
					[am_cv_python$1_pythondir=$PYTHON$1_PREFIX/lib/python$PYTHON$1_VERSION/site-packages])])])
		AC_SUBST([python$1dir], [$am_cv_python$1_pythondir])
		dnl pkgpythondir -- $PACKAGE directory under pythondir.  Was
		dnl   PYTHON_SITE_PACKAGE in previous betas, but this naming is
		dnl   more consistent with the rest of automake.
		AC_SUBST([pkgpython$1dir], [\${pythondir}/$PACKAGE])
		dnl pyexecdir -- directory for installing python extension modules
		dnl   (shared libraries)
		dnl Query distutils for this directory.
		AC_CACHE_CHECK([for $am_display_PYTHON$1 extension module directory],
			[am_cv_python$1_pyexecdir],
			[AS_IF([test "x$exec_prefix" = xNONE],
				[am_py_exec_prefix=$am_py_prefix],
				[am_py_exec_prefix=$exec_prefix])
			am_cv_python$1_pyexecdir=`$PYTHON$1 -c "PY_SNIPPET_GET_SITEDIR_PLATFORM_SPECIFIC([$am_py_prefix])"`
			AS_CASE([$am_cv_python$1_pyexecdir],
				[$am_py_exec_prefix*],
				[am__strip_prefix=`echo "$am_py_exec_prefix" | sed 's|.|.|g'`
				 am_cv_python$1_pyexecdir=`echo "$am_cv_python$1_pyexecdir" | sed "s,^$am__strip_prefix,$PYTHON$1_EXEC_PREFIX,"`],
				[*],
				[AS_CASE([$am_py_exec_prefix],
					[/usr|/System*],
					[],
					[*],
					[am_cv_python$1_pyexecdir=$PYTHON$1_EXEC_PREFIX/lib/python$PYTHON$1_VERSION/site-packages])])])
		dnl pyexecdir -- directory for installing python extension modules
		dnl   (shared libraries)
		dnl Query distutils for this directory.
		AC_SUBST([py$1execdir], [$am_cv_python$1_pyexecdir])
		dnl pkgpyexecdir -- $(pyexecdir)/$(PACKAGE)
		AC_SUBST([pkgpy$1execdir], [\${py$1execdir}/$PACKAGE])
		dnl Run any user-specified action.
		$3])
])


m4_define([_PYTHON_MAJOR_VERSION_MISMATCH_CODE], [3])
m4_define([_PYTHON_GENERAL_VERSION_MISMATCH_CODE], [2])

# AM_PYTHON_CHECK_VERSION(PROG, MAJOR_VERSION, MINIMAL_VERSION, [ACTION-IF-OK], [ACTION-IF-NOT-CORRECT-MAJOR-VERSION], [ACTION-IF-NOT-CORRECT-GENERAL-VERSION], [ACTION-IF-OTHER-ERROR])
# ---------------------------------------------------------------------------
# MAJOR_VERSION is optional, if you leave it blank, it won't be taken into account.
# MAJOR_VERSION: Optional, pass 2, 3, or nothing.
# Run ACTION-IF-NOT-CORRECT-MAJOR-VERSION if there is a problem with major version.
# Run ACTION-IF-NOT-CORRECT-GENERAL-VERSION if the required version of interpreter does not match
#     requirements, but the major version check has not been performed or it has passed.
# Run ACTION-IF-OTHER-ERROR if there was another error (e.g. Python interpreter not found)
AC_DEFUN([AM_PYTHON_CHECK_VERSION],
[
	prog="PY_SNIPPET_CHECK_VERSION([$2], _PYTHON_MAJOR_VERSION_MISMATCH_CODE, [$3], _PYTHON_GENERAL_VERSION_MISMATCH_CODE)"
	AM_RUN_LOG([$1 -c "$prog"])
	AS_IF(
		[test "$ac_status" = 0], [$4],
		m4_if([$2], , , [[test "$ac_status" = _PYTHON_MAJOR_VERSION_MISMATCH_CODE], m4_default([$5], [[$6]]),])
		[test "$ac_status" = _PYTHON_GENERAL_VERSION_MISMATCH_CODE], [$6],
		[$7])
])


# AM_CONFIGURE_PYTHON_FLAGS(ENV_STEM, PYTHON_EXECUTABLE, [IF-OK], [IF-PYTHON-CONFIG-NOT-FOUND])
#
#
AC_DEFUN([AM_CONFIGURE_PYTHON_FLAGS],
[
    AS_IF([test -x "$2-config"],
		[AC_MSG_NOTICE([Passed python-config path as absolute filename '$2-config'])
		$1_CONFIG="$2-config"],
		[AC_PATH_PROG([$1_CONFIG], [$2-config], [NONE])])
	AS_IF([test "${$1_CONFIG}" = NONE], [$4], [$3])
	$1_CFLAGS=`"${$1_CONFIG}" --cflags 2> /dev/null`
	$1_LIBS=`"${$1_CONFIG}" --libs 2> /dev/null`
	$1_INCLUDES=`"${$1_CONFIG}" --includes 2> /dev/null`
	AC_SUBST([$1_CFLAGS])
	AC_SUBST([$1_LIBS])
	AC_SUBST([$1_INCLUDES])
	AC_SUBST([$1_CONFIG])
])
