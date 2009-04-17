AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perldir = $(PERL_INCLUDES)

nobase_perl_DATA = cvsscalc.pm
perl_LTLIBRARIES = _cvsscalc_pm.la
_cvsscalc_pm_la_LDFLAGS = -module -avoid-version
nodist__cvsscalc_pm_la_SOURCES = cvsscalc_pm_wrap.c 
_cvsscalc_pm_la_LIBADD = ${top_builddir}/src/CVSS/libcvss.la

nodist_pyexec_PYTHON = cvsscalc.py
pyexec_LTLIBRARIES = _cvsscalc_py.la
_cvsscalc_py_la_LDFLAGS = -module -avoid-version
nodist__cvsscalc_py_la_SOURCES = cvsscalc_py_wrap.c 
_cvsscalc_py_la_LIBADD = ${top_builddir}/src/CVSS/libcvss.la

##beware: multiple output
cvsscalc.py cvsscalc_py_wrap.c: ${srcdir}/cvsscalc.i
	$(SWIG) -o cvsscalc_py_wrap.c -python ${INCLUDES} ${srcdir}/cvsscalc.i

cvsscalc.pm cvsscalc_pm_wrap.c: ${srcdir}/cvsscalc.i
	$(SWIG) -o cvsscalc_pm_wrap.c -perl5 ${INCLUDES} ${srcdir}/cvsscalc.i

CLEANFILES = cvsscalc.py cvsscalc_py_wrap.c cvsscalc.pyc cvsscalc_pm_wrap.c cvsscalc.pm
EXTRA_DIST = cvsscalc.i
