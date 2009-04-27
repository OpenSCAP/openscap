AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perl_vendorlib_DATA = cpe.pm
perl_vendorarch_LTLIBRARIES = _cpe_pm.la
_cpe_pm_la_LDFLAGS = -module -avoid-version
_cpe_pm_la_LIBADD = ${top_builddir}/src/CPE/libcpe.la
nodist__cpe_pm_la_SOURCES = cpe_pm_wrap.c

nodist_pyexec_PYTHON = cpe.py
pyexec_LTLIBRARIES   = _cpe_py.la
_cpe_py_la_LDFLAGS   = -module -avoid-version
_cpe_py_la_LIBADD   = ${top_builddir}/src/CPE/libcpe.la
nodist__cpe_py_la_SOURCES   = cpe_py_wrap.c

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

cpe_py_wrap.c: ${srcdir}/cpe.i
	$(SWIG) -o cpe_py_wrap.c -python -module cpe_py ${INCLUDES} ${srcdir}/cpe.i
	mv cpe_py.py cpe.py

cpe.py: cpe_py_wrap.c
	@if test -f $@; then :; else \
		rm -f cpe_py_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cpe_py_wrap.c; \
	fi

cpe_pm_wrap.c: ${srcdir}/cpe.i
	$(SWIG) -o cpe_pm_wrap.c -perl5 -module cpe_pm ${INCLUDES} ${srcdir}/cpe.i
	mv cpe_pm.pm cpe.pm

cpe.pm: cpe_pm_wrap.c
	@if test -f $@; then :; else \
		rm -f cpe_pm_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cpe_pm_wrap.c; \
	fi

CLEANFILES = cpe.py cpe_py_wrap.c cpe.pyc cpe_pm_wrap.c cpe.pm
EXTRA_DIST = cpe.i

