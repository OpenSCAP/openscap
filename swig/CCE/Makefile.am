AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perldir = $(PERL_INCLUDES)

nobase_perl_DATA = cce.pm
perl_LTLIBRARIES = _cce_pm.la
_cce_pm_la_LDFLAGS = -module -avoid-version
_cce_pm_la_LIBADD = ${top_builddir}/src/CCE/libcce.la
nodist__cce_pm_la_SOURCES = cce_pm_wrap.c

nodist_pyexec_PYTHON = cce.py
pyexec_LTLIBRARIES = _cce_py.la
_cce_py_la_LDFLAGS = -module -avoid-version
_cce_py_la_LIBADD  = ${top_builddir}/src/CCE/libcce.la
nodist__cce_py_la_SOURCES = cce_py_wrap.c

##beware: multiple output
cce.py cce_py_wrap.c: ${srcdir}/cce.i
	$(SWIG) -o cce_py_wrap.c -python -module cce_py ${INCLUDES} ${srcdir}/cce.i
	mv cce_py.py cce.py

cce.pm cce_pm_wrap.c: ${srcdir}/cce.i
	$(SWIG) -o cce_pm_wrap.c -perl5 -module cce_pm ${INCLUDES} ${srcdir}/cce.i
	mv cce_pm.pm cce.pm

CLEANFILES = cce.py cce_py_wrap.c cce.pyc cce_pm_wrap.c cce.pm
EXTRA_DIST = cce.i
