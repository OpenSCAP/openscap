AM_CPPFLAGS = -I$(top_builddir)/src/CCE -I$(top_srcdir)/src/CCE $(PYTHON_INCLUDES)

nodist_pyexec_PYTHON = cce.py
pyexec_LTLIBRARIES = _cce.la
_cce_la_LDFLAGS = -module -avoid-version
_cce_la_LIBADD  = ${top_builddir}/src/CCE/libcce.la
nodist__cce_la_SOURCES = cce_wrap.c

##beware: multiple output
cce.py cce_wrap.c: ${srcdir}/cce.i
	$(SWIG) -o cce_wrap.c -python ${INCLUDES} ${srcdir}/cce.i

CLEANFILES = cce.py cce_wrap.c cce.pyc
EXTRA_DIST = cce.i
