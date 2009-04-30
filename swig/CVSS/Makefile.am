AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perl_vendorlib_DATA = cvsscalc.pm
perl_vendorarch_LTLIBRARIES = _cvsscalc_pm.la
_cvsscalc_pm_la_LDFLAGS = -module -avoid-version
nodist__cvsscalc_pm_la_SOURCES = cvsscalc_pm_wrap.c 
_cvsscalc_pm_la_LIBADD = ${top_builddir}/src/CVSS/libcvss.la

nodist_pyexec_PYTHON = cvsscalc.py
pyexec_LTLIBRARIES = _cvsscalc_py.la
_cvsscalc_py_la_LDFLAGS = -module -avoid-version
nodist__cvsscalc_py_la_SOURCES = cvsscalc_py_wrap.c 
_cvsscalc_py_la_LIBADD = ${top_builddir}/src/CVSS/libcvss.la

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

cvsscalc_py_wrap.c: ${srcdir}/cvsscalc.i
	$(SWIG) -o cvsscalc_py_wrap.c -python -module cvsscalc_py ${INCLUDES} ${srcdir}/cvsscalc.i
	mv cvsscalc_py.py cvsscalc.py

cvsscalc.py: cvsscalc_py_wrap.c
	@if test -f $@; then :; else \
		rm -f cvsscalc_py_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cvsscalc_py_wrap.c; \
	fi

cvsscalc_pm_wrap.c: ${srcdir}/cvsscalc.i
	$(SWIG) -o cvsscalc_pm_wrap.c -perl5 -module cvsscalc_pm ${INCLUDES} ${srcdir}/cvsscalc.i
	mv cvsscalc_pm.pm cvsscalc.pm

cvsscalc.pm: cvsscalc_pm_wrap.c
	@if test -f $@; then :; else \
		rm -f cvsscalc_pm_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cvsscalc_pm_wrap.c; \
	fi

CLEANFILES = cvsscalc.py cvsscalc_py_wrap.c cvsscalc.pyc cvsscalc_pm_wrap.c cvsscalc.pm
EXTRA_DIST = cvsscalc.i
