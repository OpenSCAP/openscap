AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perldir = $(PERL_INCLUDES)

nobase_perl_DATA = cpe.pm
perl_LTLIBRARIES = _cpe_pm.la
_cpe_pm_la_LDFLAGS = -module -avoid-version
_cpe_pm_la_LIBADD = ${top_builddir}/src/CPE/libcpe.la
nodist__cpe_pm_la_SOURCES = cpe_pm_wrap.c


nodist_pyexec_PYTHON = cpe.py
pyexec_LTLIBRARIES   = _cpe_py.la
_cpe_py_la_LDFLAGS   = -module -avoid-version
_cpe_py_la_LIBADD   = ${top_builddir}/src/CPE/libcpe.la
nodist__cpe_py_la_SOURCES   = cpe_py_wrap.c

##beware: multiple output
cpe.py cpe_py_wrap.c: ${srcdir}/cpe.i
	$(SWIG) -o cpe_py_wrap.c -python -module cpe_py ${INCLUDES} ${srcdir}/cpe.i
	mv cpe_py.py cpe.py

cpe.pm cpe_pm_wrap.c: ${srcdir}/cpe.i
	$(SWIG) -o cpe_pm_wrap.c -perl5 -module cpe_pm ${INCLUDES} ${srcdir}/cpe.i
	mv cpe_pm.pm cpe.pm

CLEANFILES = cpe.py cpe_py_wrap.c cpe.pyc cpe_pm_wrap.c cpe.pm
EXTRA_DIST = cpe.i

