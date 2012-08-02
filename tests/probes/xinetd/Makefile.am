AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I${top_srcdir}/src/CVSS/public \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CCE/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
	 	-I$(top_srcdir)/src/common/public \
		-I$(top_srcdir)/src/OVAL/probes \
		-I$(top_srcdir)/src/OVAL/probes/public \
		-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
		-I$(top_srcdir)/src \
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la $(top_builddir)/src/common/liboscapcommon.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh \
             $(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS_ENVIRONMENT= LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		   LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		   OSCAP_FULL_VALIDATION=1 \
		   OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes $(SHELL) -x

TESTS = test_xinetd_probe.sh
check_PROGRAMS = test_probe_xinetd

test_probe_xinetd_SOURCES = test_probe_xinetd.c
test_probe_xinetd_CFLAGS= -I$(top_srcdir) -I$(top_srcdir)/src/common -I$(top_srcdir)/src/common/public

EXTRA_DIST += test_xinetd_probe.sh \
               test_probe_xinetd.c \
                   xinetd_A.conf \
                   xinetd_B.conf \
                   xinetd_C.conf \
                   xinetd_D.conf \
                   xinetd_E.conf \
                   xinetd_F.conf
