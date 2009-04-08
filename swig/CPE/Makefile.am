AM_CPPFLAGS = -I$(top_builddir)/src/CPE -I$(top_srcdir)/src/CPE $(PYTHON_INCLUDES)

nodist_pyexec_PYTHON = cpe.py
pyexec_LTLIBRARIES   = _cpe.la

_cpe_la_LDFLAGS   = -module -avoid-version
_cpe_la_LIBADD   = ${top_builddir}/src/CPE/libcpe.la
nodist__cpe_la_SOURCES   = cpe_wrap.c

##beware: multiple output
cpe.py cpe_wrap.c: ${srcdir}/cpe.i
	$(SWIG) -o cpe_wrap.c -python ${INCLUDES} ${srcdir}/cpe.i

CLEANFILES = cpe.py cpe_wrap.c cpe.pyc
EXTRA_DIST = cpe.i

