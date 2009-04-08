AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES)

nodist_pyexec_PYTHON = cve.py
pyexec_LTLIBRARIES = _cve.la
_cve_la_LDFLAGS = -module -avoid-version
_cve_la_LIBADD = ${top_builddir}/src/CVE/libcve.la
nodist__cve_la_SOURCES = cve_wrap.c
##beware: multiple output
cve.py cve_wrap.c: ${srcdir}/cve.i
	$(SWIG) -o cve_wrap.c -python ${INCLUDES} ${srcdir}/cve.i

CLEANFILES = cve.py cve_wrap.c cve.pyc
EXTRA_DIST = cve.i
