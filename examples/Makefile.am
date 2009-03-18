#AUTOMAKE_OPTIONS = color-tests
check_PROGRAMS =
TESTS =
TESTS_ENVIRONMENT = $(SHELL) -x

if WANT_OVAL
#check_PROGRAMS += test_oval
#test_oval_SOURCES = OVAL/test_oval.c
#test_oval_LDADD = $(top_srcdir)/libopenscap.la
endif

if WANT_CPE
check_PROGRAMS += test_cpelang test_cpedict test_cpeuri
#TESTS += CPE/test_cpe.sh

test_cpelang_SOURCES = CPE/test_cpelang.c
test_cpelang_LDADD = $(top_srcdir)/src/libopenscap.la
test_cpelang_CFLAGS = -I$(top_srcdir)/src/CPE @libxml_cflags@ @pcre_cflags@
test_cpelang_LDFLAGS = @libxml_libs@ @pcre_libs@

test_cpedict_SOURCES = CPE/test_cpedict.c
test_cpedict_CFLAGS = -I$(top_srcdir)/src/CPE @libxml_cflags@ @pcre_cflags@
test_cpedict_LDFLAGS = @libxml_libs@ @pcre_libs@
test_cpedict_LDADD = $(top_srcdir)/src/libopenscap.la

test_cpeuri_SOURCES = CPE/test_cpeuri.c
test_cpeuri_CFLAGS = -I$(top_srcdir)/src/CPE  @libxml_cflags@ @pcre_cflags@
test_cpeuri_LDFLAGS = @libxml_libs@ @pcre_libs@
test_cpeuri_LDADD = $(top_srcdir)/src/libopenscap.la
endif

if WANT_CVE
#check_PROGRAMS += test_cve
#test_cve_SOURCES = CVE/test_cve.c
#test_cve_LDADD = $(top_srcdir)/src/libopenscap.la
endif

if WANT_CCE
check_PROGRAMS += test_cce
TESTS += CCE/test_cce.sh
test_cce_SOURCES = CCE/test_cce.c
test_cce_CFLAGS = -I$(top_srcdir)/src/CCE @libxml_cflags@ @pcre_cflags@
test_cce_LDFLAGS = @libxml_libs@ @pcre_libs@
test_cce_LDADD = $(top_srcdir)/src/libopenscap.la
endif

if WANT_CVSS
#check_PROGRAMS += test_cvsscalc
#test_cvsscalc_SOURCES = CVSS/test_cvsscalc.c
#test_cvsscalc_LDADD = $(top_srcdir)/src/libopenscap.la
#EXTRA_DIST = test_cvsscalc.py
endif
