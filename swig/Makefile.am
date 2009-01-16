

AM_CFLAGS = -fPIC
PYLIBVER ?= python$(PYTHON_VERSION)
AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I/usr/include/$(PYLIBVER)

nodist_pyexec_PYTHON = cvsscalc.py
pyexec_LTLIBRARIES = _cvsscalc.la
_cvsscalc_la_LDFLAGS = -module -avoid-version
nodist__cvsscalc_la_SOURCES = cvsscalc_wrap.c ${top_srcdir}/lib/libcvsscalc.c
##beware: multiple output
cvsscalc.py cvsscalc_wrap.c: ${srcdir}/cvsscalc.i
	swig -o cvsscalc_wrap.c -python ${INCLUDES} ${srcdir}/cvsscalc.i

CLEANFILES = cvsscalc.py cvsscalc_wrap.c cvsscalc.pyc
EXTRA_DIST = cvsscalc.i

