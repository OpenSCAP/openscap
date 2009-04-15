AM_CPPFLAGS = -I$(top_builddir)/src/OVAL -I$(top_srcdir)/src/OVAL $(PYTHON_INCLUDES)

nodist_pyexec_PYTHON = oval.py
pyexec_LTLIBRARIES = _oval.la
_oval_la_LDFLAGS = -module -avoid-version
_oval_la_LIBADD  = ${top_builddir}/src/OVAL/liboval.la
nodist__oval_la_SOURCES = oval_wrap.c

##beware: multiple output
oval.py oval_wrap.c: ${srcdir}/oval.i
	$(SWIG) -o oval_wrap.c -python ${INCLUDES} ${srcdir}/oval.i

CLEANFILES = oval.py oval_wrap.c oval.pyc
EXTRA_DIST = oval.i
