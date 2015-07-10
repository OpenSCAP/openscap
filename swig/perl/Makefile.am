
AM_CFLAGS = -fPIC

AM_CPPFLAGS =	@xml2_CFLAGS@ \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CVSS/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
		-I$(top_srcdir)/src/XCCDF_POLICY/public \
		-I${top_srcdir}/src/common/public \
		-I${top_srcdir}/src/source/public \
		-I$(top_srcdir)/src/CVE/public \
		-I/usr/include -include "sys/types.h" \
		-D_LARGEFILE64_SOURCE \
		-D_LARGEFILE_SOURCE=1

CFLAGS+= -Wno-missing-prototypes

if WANT_PERL
AM_CPPFLAGS += $(PERL_INCLUDES)
perl_vendorlib_DATA = openscap.pm
perl_vendorarch_LTLIBRARIES = _openscap_pm.la
_openscap_pm_la_LDFLAGS = -module -avoid-version
_openscap_pm_la_LIBADD  = ${top_builddir}/src/libopenscap.la
nodist__openscap_pm_la_SOURCES = openscap_pm_wrap.c
endif

WANT_MODULE=
if WANT_CCE
WANT_MODULE += -DWANT_CCE
AM_CPPFLAGS += -I$(top_srcdir)/src/CCE/public
endif

##beware of: http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

if WANT_PERL
openscap_pm_wrap.c: ${srcdir}/../src/openscap.i
	$(SWIG) -o openscap_pm_wrap.c -perl5 -module openscap_pm ${INCLUDES} ${WANT_MODULE} ${srcdir}/../src/openscap.i
	mv openscap_pm.pm openscap.pm

openscap.pm: openscap_pm_wrap.c
	@if test -f $@; then :; else \
		rm -f openscap_pm_wrap.c; \
		$(MAKE) $(AM_MAKEFLAGS) openscap_pm_wrap.c; \
	fi
endif

CLEANFILES = openscap.pm openscap_pm_wrap.c
