if WANT_XCCDF

AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I$(top_srcdir)/src/CVSS/public \
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

TESTS = test_api_xccdf.sh
check_PROGRAMS = test_api_xccdf_dump test_api_xccdf

test_api_xccdf_dump_SOURCES = test_api_xccdf_dump.c
test_api_xccdf_SOURCES = test_api_xccdf.c

EXTRA_DIST += test_api_xccdf.sh scap-rhel5-xccdf.xml 

endif
