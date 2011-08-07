if WANT_PROBES

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

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh \
             $(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS_ENVIRONMENT= LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so LD_LIBRARY_PATH=$(top_builddir)/src/.libs OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes $(SHELL) -x
TESTS = test_api_probes.sh fts.sh
check_PROGRAMS = test_api_probes_smoke oval_fts_list

test_api_probes_smoke_SOURCES = test_api_probes_smoke.c
oval_fts_list_CFLAGS= -I$(top_srcdir)/src/OVAL/probes
oval_fts_list_SOURCES= oval_fts_list.c

EXTRA_DIST += test_api_probes.sh \
              test_api_probes_smoke.c	\
              fts.sh	\
              gentree.sh

endif
