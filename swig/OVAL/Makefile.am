AM_CPPFLAGS = -I$(top_builddir)/src/CVE -I$(top_srcdir)/src/CVE $(PYTHON_INCLUDES) -I$(PERL_INCLUDES) -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE=1 -iquote -I/usr/include -include "sys/types.h"

perl_vendorlib_DATA = oval.pm
perl_vendorarch_LTLIBRARIES = _oval_pm.la
_oval_pm_la_LDFLAGS = -module -avoid-version
_oval_pm_la_LIBADD  = ${top_builddir}/src/OVAL/liboval.la
nodist__oval_pm_la_SOURCES = oval_pm_wrap.c

nodist_pyexec_PYTHON = oval.py
pyexec_LTLIBRARIES = _oval_py.la
_oval_py_la_LDFLAGS = -module -avoid-version
_oval_py_la_LIBADD  = ${top_builddir}/src/OVAL/liboval.la
nodist__oval_py_la_SOURCES = oval_py_wrap.c

##beware: multiple output
oval.py oval_py_wrap.c: ${srcdir}/oval.i
	$(SWIG) -o oval_py_wrap.c -python -module oval_py ${INCLUDES} ${srcdir}/oval.i
	mv oval_py.py oval.py

oval.pm oval_pm_wrap.c: ${srcdir}/oval.i
	$(SWIG) -o oval_pm_wrap.c -perl5 -module oval_pm ${INCLUDES} ${srcdir}/oval.i
	mv oval_pm.pm oval.pm

CLEANFILES = oval.py oval_py_wrap.c oval.pyc oval_pm_wrap.c oval.pm
EXTRA_DIST = oval.i

