
AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I$(top_srcdir)/src/CVSS/public \
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

DISTCLEANFILES = *.log *.out* oscap_debug.log* exported*
CLEANFILES = *.log *.out* oscap_debug.log* exported*

TESTS = test_api_oval.sh

check_PROGRAMS = test_api_oval test_api_syschar test_api_results test_api_directives

test_api_oval_SOURCES = test_api_oval.c
test_api_syschar_SOURCES = test_api_syschar.c
test_api_results_SOURCES = test_api_results.c
test_api_directives_SOURCES = test_api_directives.c

EXTRA_DIST = test_api_oval.sh \
	      scap-rhel5-oval.xml \
	      composed-oval.xml \
	      system-characteristics.xml \
	      results.xml \
              directives.xml \
              results-good.xml

SUBDIRS = \
	glob_to_regex \
	schema_version \
	report_variable_values \
	unittests \
	validate
