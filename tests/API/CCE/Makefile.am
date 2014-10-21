if WANT_CCE

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

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS = test_api_cce.sh
check_PROGRAMS = test_api_cce

test_api_cce_SOURCES = test_api_cce.c

EXTRA_DIST = test_api_cce.sh        \
              test_api_cce.c         \
	      cce-sample.xml         \
              cce-sample-invalid.xml \
              cce-sample-damaged.xml \
              cce-1.txt              \
              cce-2.txt              \
              cce.txt                
endif
