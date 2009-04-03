AM_CPPFLAGS = -I$(top_builddir)/src/CPE -I$(top_srcdir)/src/CPE $(PYTHON_INCLUDES)

nodist_pyexec_PYTHON = cpeuri.py cpelang.py cpematch.py cpedict.py
pyexec_LTLIBRARIES   = _cpeuri.la _cpelang.la _cpematch.la _cpedict.la

_cpeuri_la_LDFLAGS   = -module -avoid-version
_cpelang_la_LDFLAGS  = -module -avoid-version
_cpematch_la_LDFLAGS = -module -avoid-version
_cpedict_la_LDFLAGS  = -module -avoid-version

nodist__cpeuri_la_SOURCES   = cpeuri_wrap.c ${top_srcdir}/src/CPE/cpeuri.c
nodist__cpelang_la_SOURCES  = cpelang_wrap.c ${top_srcdir}/src/CPE/cpelang.c
nodist__cpematch_la_SOURCES = cpematch_wrap.c ${top_srcdir}/src/CPE/cpematch.c
nodist__cpedict_la_SOURCES  = cpedict_wrap.c ${top_srcdir}/src/CPE/cpedict.c

##beware: multiple output
cpeuri.py cpeuri_wrap.c: ${srcdir}/cpeuri.i
	$(SWIG) -o cpeuri_wrap.c -python ${INCLUDES} ${srcdir}/cpeuri.i

cpelang.py cpelang_wrap.c: ${srcdir}/cpelang.i
	$(SWIG) -o cpelang_wrap.c -python ${INCLUDES} ${srcdir}/cpelang.i

cpematch.py cpematch_wrap.c: ${srcdir}/cpematch.i
	$(SWIG) -o cpematch_wrap.c -python ${INCLUDES} ${srcdir}/cpematch.i

cpedict.py cpedict_wrap.c: ${srcdir}/cpedict.i
	$(SWIG) -o cpedict_wrap.c -python ${INCLUDES} ${srcdir}/cpedict.i

CLEANFILES = cpeuri.py cpelang.py cpedict.py cpematch.py
CLEANFILES += cpeuri_wrap.c cpelang_wrap.c cpedict_wrap.c cpematch_wrap.c 
CLEANFILES += cpeuri.pyc cpelang.pyc cpedict.pyc cpematch.pyc
EXTRA_DIST = cpeuri.i cpelang.i cpedict.i cpematch.i
