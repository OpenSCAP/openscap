if WANT_CVSS

AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I${top_srcdir}/src/CVSS/public \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CCE/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
	 	-I$(top_srcdir)/src/common/public \
		-I$(top_srcdir)/src/OVAL/probes/public \
		-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
		-I$(top_srcdir)/src \
		@libxml_cflags@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_libs@

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh  

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS = test_api_cvss.sh
check_PROGRAMS = test_api_cvss

test_api_cvss_SOURCES = test_api_cvss.c
test_api_cvss_CFLAGS  = -ffloat-store

EXTRA_DIST += test_api_cvss.sh \
              test_api_cvss.c
endif

