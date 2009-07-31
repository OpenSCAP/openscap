#AUTOMAKE_OPTIONS = color-tests
check_PROGRAMS =
TESTS =
TESTS_ENVIRONMENT = $(SHELL) -x
EXTRA_DIST =

if WANT_OVAL
check_PROGRAMS += test_oval test_syschar
TESTS += OVAL/test_oval.sh OVAL/test_syschar.sh
EXTRA_DIST += OVAL/test_oval.sh OVAL/test.xml OVAL/test_syschar.sh

test_oval_SOURCES = OVAL/test_oval.c
test_oval_CFLAGS = -I$(top_srcdir)/src/OVAL
test_oval_LDADD = $(top_builddir)/src/libopenscap.la

test_syschar_SOURCES = OVAL/test_syschar.c
test_syschar_CFLAGS = -I$(top_srcdir)/src/OVAL
test_syschar_LDADD = $(top_builddir)/src/libopenscap.la

if WANT_PROBES
check_PROGRAMS += sexp_test  test_findfile test_rpminfo
TESTS += SEAP/parser_valid-test.sh SEAP/parser_invalid-test.sh
EXTRA_DIST += SEAP/parser_valid-test.sh SEAP/parser_invalid-test.sh

test_findfile_SOURCES = OVAL/test_findfile.c
test_findfile_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src/OVAL/probes -I$(top_srcdir)/src/OVAL
test_findfile_LDADD = $(top_builddir)/src/libopenscap.la

sexp_test_SOURCES = SEAP/sexp_test.c
sexp_test_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public
sexp_test_LDADD = $(top_builddir)/src/libopenscap.la

test_rpminfo_SOURCES = OVAL/test_rpminfo.c
test_rpminfo_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src/OVAL
test_rpminfo_LDADD = $(top_builddir)/src/libopenscap.la

endif
endif

if WANT_CPE
check_PROGRAMS += test_cpelang test_cpedict test_cpeuri
TESTS += CPE/test_cpe.sh
EXTRA_DIST += CPE/test_cpe.sh CPE/dict.xml CPE/lang.xml

test_cpelang_SOURCES = CPE/test_cpelang.c
test_cpelang_LDADD = $(top_builddir)/src/libopenscap.la
test_cpelang_CFLAGS = -I$(top_srcdir)/src/CPE

test_cpedict_SOURCES = CPE/test_cpedict.c
test_cpedict_CFLAGS = -I$(top_srcdir)/src/CPE
test_cpedict_LDADD = $(top_builddir)/src/libopenscap.la

test_cpeuri_SOURCES = CPE/test_cpeuri.c
test_cpeuri_CFLAGS = -I$(top_srcdir)/src/CPE
test_cpeuri_LDADD = $(top_builddir)/src/libopenscap.la
endif

if WANT_CVE
check_PROGRAMS += test_cve
TESTS += CVE/test_cve.sh
EXTRA_DIST += CVE/test_cve.sh CVE/nvdcve-2.0-recent.xml

test_cve_SOURCES = CVE/test_cve.c
test_cve_CFLAGS = -I$(top_srcdir)/src/CVE
test_cve_LDADD = $(top_builddir)/src/libopenscap.la
endif

if WANT_CCE
check_PROGRAMS += test_cce
TESTS += CCE/test_cce.sh
EXTRA_DIST += CCE/test_cce.sh CCE/CCE_Sample.xml

test_cce_SOURCES = CCE/test_cce.c
test_cce_CFLAGS = -I$(top_srcdir)/src/CCE
test_cce_LDADD = $(top_builddir)/src/libopenscap.la
endif

if WANT_CVSS
check_PROGRAMS += test_cvsscalc
TESTS += test_cvsscalc
test_cvsscalc_SOURCES = CVSS/test_cvsscalc.c
test_cvsscalc_CFLAGS  = -I$(top_srcdir)/src/CVSS
test_cvsscalc_LDADD = $(top_builddir)/src/libopenscap.la
EXTRA_DIST += CVSS/test_cvsscalc.py
endif

if WANT_XCCDF
check_PROGRAMS += xccdf_dump
TESTS += XCCDF/test_xccdf.sh
EXTRA_DIST += XCCDF/test_xccdf.sh XCCDF/scap-rhel5-xccdf.xml

xccdf_dump_SOURCES = XCCDF/xccdf_dump.c
xccdf_dump_CFLAGS = -I$(top_srcdir)/src/XCCDF
xccdf_dump_LDADD = $(top_builddir)/src/libopenscap.la
endif

