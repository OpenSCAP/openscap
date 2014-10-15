AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/CCE/public \
	-I$(top_srcdir)/src/CPE/public \
	-I$(top_srcdir)/src/CVE/public \
	-I${top_srcdir}/src/CVSS/public \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
	-I$(top_srcdir)/src/OVAL/probes/public \
	-I$(top_srcdir)/src/OVAL/public \
	-I$(top_srcdir)/src/XCCDF/public \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/source/public \
	-I$(top_srcdir)/tests/include \
	@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT = \
		$(top_builddir)/run
TESTS = all.sh
check_PROGRAMS = test_api_probes_smoke oval_fts_list

test_api_probes_smoke_SOURCES = test_api_probes_smoke.c
oval_fts_list_CFLAGS= -I$(top_srcdir)/src/OVAL/probes
oval_fts_list_SOURCES= oval_fts_list.c

EXTRA_DIST += \
	all.sh \
	fts.sh \
	gentree.sh \
	test_api_probes_smoke.c
