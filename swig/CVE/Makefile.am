AM_CPPFLAGS = -I${top_srcdir}/src/common/public -I${top_srcdir}/src/CVE/public -I$(top_srcdir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perl_vendorlib_DATA = cve.pm
perl_vendorarch_LTLIBRARIES = _cve_pm.la
_cve_pm_la_LDFLAGS = -module -avoid-version
_cve_pm_la_LIBADD = ${top_builddir}/src/CVE/libcve.la ${top_builddir}/src/common/liboscapcommon.la
nodist__cve_pm_la_SOURCES = cve_pm_wrap.c

nodist_pyexec_PYTHON = cve.py
pyexec_LTLIBRARIES = _cve_py.la
_cve_py_la_LDFLAGS = -module -avoid-version
_cve_py_la_LIBADD = ${top_builddir}/src/CVE/libcve.la ${top_builddir}/src/common/liboscapcommon.la
nodist__cve_py_la_SOURCES = cve_py_wrap.c

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

cve_py_wrap.c: ${srcdir}/cve.i
	$(SWIG) -o cve_py_wrap.c -python -module cve_py ${INCLUDES} ${srcdir}/cve.i
	mv cve_py.py cve.py

cve.py: cve_py_wrap.c
	@if test -f $@; then :; else \
		rm -f cve_py_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cve_py_wrap.c; \
	fi

cve_pm_wrap.c: ${srcdir}/cve.i
	$(SWIG) -o cve_pm_wrap.c -perl5 -module cve_pm ${INCLUDES} ${srcdir}/cve.i
	mv cve_pm.pm cve.pm

cve.pm: cve_pm_wrap.c
	@if test -f $@; then :; else \
		rm -f cve_pm_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) cve_pm_wrap.c; \
	fi


CLEANFILES = cve.py cve_py_wrap.c cve_pm_wrap.c cve.pyc cve.pm
EXTRA_DIST = cve.i
