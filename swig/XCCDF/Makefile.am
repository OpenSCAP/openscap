AM_CPPFLAGS =-I${top_srcdir}/src/common/public -I$(top_srcdir)/src/XCCDF/public -I$(top_srcdir)/src/XCCDF $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perl_vendorlib_DATA = xccdf.pm
perl_vendorarch_LTLIBRARIES = _xccdf_pm.la
_xccdf_pm_la_LDFLAGS = -module -avoid-version
_xccdf_pm_la_LIBADD  = ${top_builddir}/src/XCCDF/libxccdf.la ${top_builddir}/src/common/liboscapcommon.la
nodist__xccdf_pm_la_SOURCES = xccdf_pm_wrap.c

nodist_pyexec_PYTHON = xccdf.py
pyexec_LTLIBRARIES = _xccdf_py.la
_xccdf_py_la_LDFLAGS = -module -avoid-version
_xccdf_py_la_LIBADD  = ${top_builddir}/src/XCCDF/libxccdf.la ${top_builddir}/src/common/liboscapcommon.la
nodist__xccdf_py_la_SOURCES = xccdf_py_wrap.c

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

xccdf_py_wrap.c: ${srcdir}/xccdf.i
	$(SWIG) -o xccdf_py_wrap.c -python -module xccdf_py ${INCLUDES} ${srcdir}/xccdf.i
	mv xccdf_py.py xccdf.py

xccdf.py: xccdf_py_wrap.c
	@if test -f $@; then :; else \
		rm -f xccdf_py_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) xccdf_py_wrap.c; \
	fi

xccdf_pm_wrap.c: ${srcdir}/xccdf.i
	$(SWIG) -o xccdf_pm_wrap.c -perl5 -module xccdf_pm ${INCLUDES} ${srcdir}/xccdf.i
	mv xccdf_pm.pm xccdf.pm

xccdf.pm: xccdf_pm_wrap.c
	@if test -f $@; then :; else \
		rm -f xccdf_pm_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) xccdf_pm_wrap.c; \
	fi

CLEANFILES = xccdf.py xccdf_py_wrap.c xccdf.pyc xccdf_pm_wrap.c xccdf.pm
EXTRA_DIST = xccdf.i

