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
		-I$(top_srcdir)/src/source/public \
		-I$(top_srcdir)/src \
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_oscap_string.sh
check_PROGRAMS = test_oscap_string

test_oscap_string_SOURCES = test_oscap_string.c
test_oscap_string_SOURCES += $(top_srcdir)/src/common/oscap_string.c $(top_srcdir)/src/common/oscap_buffer.c $(top_srcdir)/src/common/alloc.c
test_oscap_common_CPPFLAGS = $(AM_CPPFLAGS) -DNDEBUG

EXTRA_DIST = test_oscap_string.sh \
              test_oscap_string.c

