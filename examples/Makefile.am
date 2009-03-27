#AUTOMAKE_OPTIONS = color-tests
check_PROGRAMS =
TESTS =
TESTS_ENVIRONMENT = $(SHELL) -x
EXTRA_DIST =

if WANT_OVAL
#check_PROGRAMS += test_oval
#test_oval_SOURCES = OVAL/test_oval.c
#test_oval_LDADD = $(top_srcdir)/libopenscap.la
endif

if WANT_CPE
check_PROGRAMS += test_cpelang test_cpedict test_cpeuri
TESTS += CPE/test_cpe.sh
EXTRA_DIST += CPE/test_cpe.sh CPE/dict.xml

test_cpelang_SOURCES = CPE/test_cpelang.c
test_cpelang_LDADD = $(top_srcdir)/src/libopenscap.la
test_cpelang_CFLAGS = -I$(top_srcdir)/src/CPE

test_cpedict_SOURCES = CPE/test_cpedict.c
test_cpedict_CFLAGS = -I$(top_srcdir)/src/CPE
test_cpedict_LDADD = $(top_srcdir)/src/libopenscap.la

test_cpeuri_SOURCES = CPE/test_cpeuri.c
test_cpeuri_CFLAGS = -I$(top_srcdir)/src/CPE
test_cpeuri_LDADD = $(top_srcdir)/src/libopenscap.la
endif

if WANT_CVE
check_PROGRAMS += test_cve
TESTS += CVE/test_cve.sh
EXTRA_DIST += CVE/test_cve.sh CVE/nvdcve-2.0-recent.xml

test_cve_SOURCES = CVE/test_cve.c
test_cve_CFLAGS = -I$(top_srcdir)/src/CVE
test_cve_LDADD = $(top_srcdir)/src/libopenscap.la
endif

if WANT_CCE
check_PROGRAMS += test_cce
TESTS += CCE/test_cce.sh
EXTRA_DIST += CCE/test_cce.sh CCE/CCE_Sample.xml

test_cce_SOURCES = CCE/test_cce.c
test_cce_CFLAGS = -I$(top_srcdir)/src/CCE
test_cce_LDADD = $(top_srcdir)/src/libopenscap.la
endif

if WANT_CVSS
check_PROGRAMS += test_cvsscalc
TESTS += test_cvsscalc
test_cvsscalc_SOURCES = CVSS/test_cvsscalc.c
test_cvsscalc_CFLAGS  = -I$(top_srcdir)/src/CVSS
test_cvsscalc_LDADD = $(top_srcdir)/src/libopenscap.la
EXTRA_DIST += CVSS/test_cvsscalc.py
endif
