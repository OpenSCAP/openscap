AM_CPPFLAGS = -I${top_srcdir}/src/common/public -I${top_srcdir}/src/CCE/public $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perl_vendorlib_DATA = cce.pm
perl_vendorarch_LTLIBRARIES = _cce_pm.la
_cce_pm_la_LDFLAGS = -module -avoid-version
_cce_pm_la_LIBADD = ${top_builddir}/src/CCE/libcce.la ${top_builddir}/src/common/liboscapcommon.la
nodist__cce_pm_la_SOURCES = cce_pm_wrap.c

nodist_pyexec_PYTHON = cce.py
pyexec_LTLIBRARIES = _cce_py.la
_cce_py_la_LDFLAGS = -module -avoid-version
_cce_py_la_LIBADD  = ${top_builddir}/src/CCE/libcce.la ${top_builddir}/src/common/liboscapcommon.la
nodist__cce_py_la_SOURCES = cce_py_wrap.c

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

cce_py_wrap.c: ${srcdir}/cce.i
	$(SWIG) -o cce_py_wrap.c -python -module cce_py ${INCLUDES} ${srcdir}/cce.i
	mv cce_py.py cce.py

cce.py: cce_py_wrap.c
	@if test -f $@; then :; else \
		rm -f cce_py_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cce_py_wrap.c; \
	fi

cce_pm_wrap.c: ${srcdir}/cce.i
	$(SWIG) -o cce_pm_wrap.c -perl5 -module cce_pm ${INCLUDES} ${srcdir}/cce.i
	mv cce_pm.pm cce.pm

cce.pm: cce_pm_wrap.c
	@if test -f $@; then :; else \
		rm -f cce_pm_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cce_pm_wrap.c; \
	fi

CLEANFILES = cce.py cce_py_wrap.c cce.pyc cce_pm_wrap.c cce.pm
EXTRA_DIST = cce.i
