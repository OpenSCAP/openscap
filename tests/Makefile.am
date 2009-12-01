check_PROGRAMS =
TESTS =
TESTS_ENVIRONMENT = $(SHELL)
EXTRA_DIST =
DISTCLEANFILES = *.log *.out

CFLAGS += @libxml_cflags@

if WANT_OVAL
check_PROGRAMS += test_oval \
	          test_syschar \
		  test_results

TESTS += OVAL/test_oval.sh 

EXTRA_DIST += OVAL/test_oval.sh \
	      OVAL/scap-rhel5-oval.xml \
	      OVAL/composed-oval.xml \
	      OVAL/system-characteristics.xml \
	      OVAL/results.xml

test_oval_SOURCES = OVAL/test_oval.c
test_oval_CFLAGS = -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/common
test_oval_LDADD = $(top_builddir)/src/libopenscap_testing.la

test_syschar_SOURCES = OVAL/test_syschar.c
test_syschar_CFLAGS = -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/common
test_syschar_LDADD = $(top_builddir)/src/libopenscap_testing.la

test_results_SOURCES = OVAL/test_results.c
test_results_CFLAGS = -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/common
test_results_LDADD = $(top_builddir)/src/libopenscap_testing.la

if WANT_PROBES
check_PROGRAMS += sexp_parser test_probes test_probe-api test_sysinfo
#test_findfile test_sexp_to_syschar
TESTS += OVAL/SEAP/test_seap.sh \
	 OVAL/probes/test_probes.sh 

EXTRA_DIST += OVAL/SEAP/test_seap.sh 	  \
	      OVAL/probes/test_probes.sh  \
	      OVAL/probes/rpminfo.xml 	  \
	      OVAL/probes/runlevel.xml	  \
	      OVAL/probes/filecontent.xml \
	      OVAL/probes/file-set.xml	  \
	      OVAL/probes/file-set2.xml	  \
	      OVAL/probes/file-set3.xml	  \
	      OVAL/probes/file.xml	  

sexp_parser_SOURCES = OVAL/SEAP/sexp_parser.c
sexp_parser_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public
sexp_parser_LDADD = $(top_builddir)/src/libopenscap_testing.la

check_PROGRAMS += sexp_string
sexp_string_SOURCES = OVAL/SEAP/sexp_string.c
sexp_string_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public
sexp_string_LDADD = $(top_builddir)/src/libopenscap_testing.la

check_PROGRAMS += sexp_number
sexp_number_SOURCES = OVAL/SEAP/sexp_number.c
sexp_number_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public
sexp_number_LDADD = $(top_builddir)/src/libopenscap_testing.la

check_PROGRAMS += sexp_list
sexp_list_SOURCES = OVAL/SEAP/sexp_list.c
sexp_list_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public
sexp_list_LDADD = $(top_builddir)/src/libopenscap_testing.la

test_probes_SOURCES = OVAL/probes/test_probes.c
test_probes_CFLAGS = -I$(top_srcdir)/src/OVAL
test_probes_LDADD = $(top_builddir)/src/libopenscap_testing.la

test_probe_api_SOURCES = OVAL/probes/test_probe-api.c
test_probe_api_CFLAGS = -I$(top_srcdir)/src/OVAL/public -I$(top_srcdir)/src/OVAL/probes/public -I$(top_srcdir)/src/OVAL/probes/SEAP/public
test_probe_api_LDADD = $(top_builddir)/src/libopenscap_testing.la

test_sysinfo_SOURCES = OVAL/probes/test_sysinfo.c
test_sysinfo_CFLAGS = -I$(top_srcdir)/src/OVAL
test_sysinfo_LDADD = $(top_builddir)/src/libopenscap_testing.la

#test_findfile_SOURCES = OVAL/test_findfile.c
#test_findfile_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src/OVAL/probes -I$(top_srcdir)/src/OVAL
#test_findfile_LDADD = $(top_builddir)/src/libopenscap_testing.la

#test_sexp_to_syschar_SOURCES = OVAL/test_sexp_to_syschar.c
#test_sexp_to_syschar_CFLAGS = -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/OVAL/probes
#test_sexp_to_syschar_LDADD = $(top_builddir)/src/libopenscap_testing.la

endif
endif

if WANT_CPE
check_PROGRAMS += test_cpelang test_cpedict test_cpeuri
TESTS += CPE/test_cpeuri.sh  \
	 CPE/test_cpedict.sh \
	 CPE/test_cpelang.sh
EXTRA_DIST += test_common.sh CPE/test_cpeuri.sh CPE/test_cpedict.sh CPE/test_cpelang.sh CPE/dict.xml CPE/lang.xml CPE/lang-corrupted.xml CPE/lang-copy.xml

test_cpelang_SOURCES = CPE/test_cpelang.c
test_cpelang_LDADD = $(top_builddir)/src/libopenscap_testing.la
test_cpelang_CFLAGS = -I$(top_srcdir)/src/CPE -I${top_srcdir}/src/CPE/public -I${top_srcdir}/src/common/public

test_cpedict_SOURCES = CPE/test_cpedict.c
test_cpedict_CFLAGS = -I$(top_srcdir)/src/CPE -I${top_srcdir}/src/CPE/public -I${top_srcdir}/src/common/public
test_cpedict_LDADD = $(top_builddir)/src/libopenscap_testing.la

test_cpeuri_SOURCES = CPE/test_cpeuri.c
test_cpeuri_CFLAGS = -I$(top_srcdir)/src/CPE -I${top_srcdir}/src/CPE/public -I${top_srcdir}/src/common/public
test_cpeuri_LDADD = $(top_builddir)/src/libopenscap_testing.la
endif

if WANT_CVE
check_PROGRAMS += test_cve
TESTS += CVE/test_cve.sh
EXTRA_DIST += CVE/test_cve.sh CVE/nvdcve-2.0-recent.xml

test_cve_SOURCES = CVE/test_cve.c
test_cve_CFLAGS = -I$(top_srcdir)/src/CVE/public -I${top_srcdir}/src/CVE \
		  -I${top_srcdir}/src/common/public -I$(top_srcdir)/src/CPE/public 
test_cve_LDADD = $(top_builddir)/src/libopenscap_testing.la
endif

if WANT_CCE
check_PROGRAMS += test_cce
TESTS += CCE/test_cce.sh
EXTRA_DIST += CCE/test_cce.sh CCE/CCE_Sample.xml

test_cce_SOURCES = CCE/test_cce.c
test_cce_CFLAGS = -I$(top_srcdir)/src/CCE/public -I${top_srcdir}/src/common/public
test_cce_LDADD = $(top_builddir)/src/libopenscap_testing.la
endif

if WANT_CVSS
check_PROGRAMS += test_cvsscalc
TESTS += CVSS/test_cvsscalc.sh
EXTRA_DIST += CVSS/test_cvsscalc.sh
test_cvsscalc_SOURCES = CVSS/test_cvsscalc.c
test_cvsscalc_CFLAGS  = -I$(top_srcdir)/src/CVSS/public -I${top_srcdir}/src/common/public
test_cvsscalc_LDADD = $(top_builddir)/src/libopenscap_testing.la
EXTRA_DIST += CVSS/test_cvsscalc.py
endif

if WANT_XCCDF
check_PROGRAMS += xccdf_dump
TESTS += XCCDF/test_xccdf.sh
EXTRA_DIST += XCCDF/test_xccdf.sh XCCDF/scap-rhel5-xccdf.xml

xccdf_dump_SOURCES = XCCDF/xccdf_dump.c
xccdf_dump_CFLAGS = -I$(top_srcdir)/src/XCCDF/public -I${top_srcdir}/src/common/public
xccdf_dump_LDADD = $(top_builddir)/src/libopenscap_testing.la
endif
