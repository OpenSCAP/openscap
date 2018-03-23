
AM_CFLAGS = -fPIC
PYLIBVER ?= python$(PYTHON2_VERSION)

AM_CPPFLAGS =	@xml2_CFLAGS@ \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CVSS/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
		-I$(top_srcdir)/src/XCCDF_POLICY/public \
		-I${top_srcdir}/src/common/public \
		-I${top_srcdir}/src/source/public \
		-I$(top_srcdir)/src/CVE/public \
		-I$(top_srcdir)/src/DS/public \
		-I/usr/include -include "sys/types.h" \
		-D_LARGEFILE64_SOURCE \
		-D_LARGEFILE_SOURCE=1

CFLAGS+= -Wno-missing-prototypes

if WANT_PYTHON2
AM_CPPFLAGS += $(PYTHON2_INCLUDES)
nodist_py2exec_PYTHON = openscap.py ${srcdir}/../src/openscap_api.py
py2exec_LTLIBRARIES = _openscap_py.la
_openscap_py_la_LDFLAGS = -module -avoid-version
_openscap_py_la_LIBADD  = ${top_builddir}/src/libopenscap.la
nodist__openscap_py_la_SOURCES = openscap_py_wrap.c
endif

WANT_MODULE=
if WANT_CCE
WANT_MODULE += -DWANT_CCE
AM_CPPFLAGS += -I$(top_srcdir)/src/CCE/public
endif

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

if WANT_PYTHON2
openscap_py_wrap.c: ${srcdir}/../src/openscap.i
	$(SWIG) -o openscap_py_wrap.c -python -module openscap_py ${INCLUDES} ${WANT_MODULE} ${srcdir}/../src/openscap.i
	mv openscap_py.py openscap.py

openscap.py: openscap_py_wrap.c
	@if test -f $@; then :; else \
		rm -f openscap_py_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) openscap_py_wrap.c; \
	fi
endif

CLEANFILES = openscap.py openscap_py_wrap.c openscap.pyc
EXTRA_DIST = ${srcdir}/../src/openscap_api.py
