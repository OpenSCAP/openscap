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
                -I$(top_srcdir)/src/OVAL/probes/SEAP/generic \
                -I$(top_srcdir)/src \
                @xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_api_seap.sh
check_PROGRAMS = test_api_seap_concurency \
                 test_api_seap_list       \
                 test_api_seap_number     \
                 test_api_seap_spb        \
                 test_api_seap_string     \
                 test_api_seap_parser	  \
		 test_api_sexp_ID	  \
		 test_api_SEXP_deepcmp    \
		 test_api_strto

test_api_seap_parser_SOURCES     = test_api_seap_parser.c
test_api_sexp_ID_SOURCES         = test_api_sexp_ID.c
test_api_seap_string_SOURCES     = test_api_seap_string.c
test_api_seap_number_SOURCES     = test_api_seap_number.c
test_api_seap_list_SOURCES       = test_api_seap_list.c
test_api_seap_concurency_SOURCES = test_api_seap_concurency.c
test_api_seap_concurency_CFLAGS  = @PTHREAD_CFLAGS@
test_api_seap_concurency_LDFLAGS = @PTHREAD_LIBS@
test_api_seap_spb_SOURCES        = test_api_seap_spb.c
test_api_SEXP_deepcmp_SOURCES    = test_api_SEXP_deepcmp.c
test_api_strto_SOURCES		 = test_api_strto.c

EXTRA_DIST += test_api_seap.sh           \
              test_api_seap_parser.c     \
	      test_api_sexp_ID.c	 \
              test_api_seap_string.c     \
              test_api_seap_number.c     \
              test_api_seap_list.c       \
              test_api_seap_concurency.c \
	      test_api_SEXP_deepcmp.c    \
	      test_api_strto.c
