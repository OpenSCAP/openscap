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

DISTCLEANFILES = *.log
CLEANFILES = *.log

TESTS = test_api_crypt.sh

check_PROGRAMS = test_crapi_digest \
	 	 test_crapi_mdigest 

test_crapi_digest_SOURCES= test_crapi_digest.c
test_crapi_digest_CFLAGS= -I$(top_srcdir)/src/OVAL/probes/
test_crapi_digest_LDFLAGS= $(top_builddir)/src/OVAL/probes/crapi/libcrapi.la

test_crapi_mdigest_SOURCES= test_crapi_mdigest.c
test_crapi_mdigest_CFLAGS= -I$(top_srcdir)/src/OVAL/probes/
test_crapi_mdigest_LDFLAGS= $(top_builddir)/src/OVAL/probes/crapi/libcrapi.la

EXTRA_DIST = test_api_crypt.sh    \
	      test_crapi_digest.c  \
	      test_crapi_mdigest.c
