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
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh \
             $(top_srcdir)/tests/xmldiff.pl    

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS = test_api_cpe_lang.sh
check_PROGRAMS = test_api_cpe_lang

test_api_cpe_lang_SOURCES = test_api_cpe_lang.c

EXTRA_DIST += test_api_cpe_lang.sh \
              test_api_cpe_lang.c \
	      lang-copy.xml       \
              lang-corrupted.xml  \
              lang-damaged.xml    \
              lang.xml	
