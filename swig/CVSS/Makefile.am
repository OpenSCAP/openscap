AM_CPPFLAGS = -I$(top_builddir)/src/CVSS -I$(top_srcdir)/src/CVSS $(PYTHON_INCLUDES)

nodist_pyexec_PYTHON = cvsscalc.py
pyexec_LTLIBRARIES = _cvsscalc.la
_cvsscalc_la_LDFLAGS = -module -avoid-version
nodist__cvsscalc_la_SOURCES = cvsscalc_wrap.c 
_cvsscalc_la_LIBADD = ${top_builddir}/src/CVSS/libcvss.la
#nodist__cvsscalc_la_SOURCES = cvsscalc_wrap.c ${top_srcdir}/src/CVSS/cvsscalc.c
##beware: multiple output
cvsscalc.py cvsscalc_wrap.c: ${srcdir}/cvsscalc.i
	$(SWIG) -o cvsscalc_wrap.c -python ${INCLUDES} ${srcdir}/cvsscalc.i

CLEANFILES = cvsscalc.py cvsscalc_wrap.c cvsscalc.pyc
EXTRA_DIST = cvsscalc.i
