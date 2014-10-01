AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I${top_srcdir}/src/CVSS/public \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CCE/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
	 	-I$(top_srcdir)/src/common/public \
		-I$(top_srcdir)/src/source/public \
		-I$(top_srcdir)/src/OVAL/probes/public \
		-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
		-I$(top_srcdir)/src \
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh \
             $(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log oscap_debug.log.*
CLEANFILES = *.log oscap_debug.log.*

TESTS_ENVIRONMENT= \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_sysinfo.sh

check_PROGRAMS = test_probes_sysinfo

test_probes_sysinfo_SOURCES = test_probes_sysinfo.c
test_probes_sysinfo_CFLAGS = -I$(top_srcdir)/src/OVAL @xml2_CFLAGS@

EXTRA_DIST += test_probes_sysinfo.sh
