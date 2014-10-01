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

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS = test_api_cpe_dict.sh
check_PROGRAMS = test_api_cpe_dict

test_api_cpe_dict_SOURCES = test_api_cpe_dict.c

TESTS_ENVIRONMENT= \
	builddir=$(top_builddir) \
	$(top_builddir)/run

EXTRA_DIST += test_api_cpe_dict.sh \
              test_api_cpe_dict.c  \
	      dict-cp1250-dos.xml  \
              dict-damaged.xml     \
	dict.xml				\
	official-cpe-dictionary_v2.2.xml	\
	official-cpe-dictionary_v2.3.xml
