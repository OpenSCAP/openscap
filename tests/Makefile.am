

TESTS =
check_PROGRAMS =
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
		-I$(top_srcdir)/src

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_libs@
EXTRA_DIST = test_common.sh   \
             xmldiff.pl       \
	     include/assume.h 

DISTCLEANFILES = *.log *.out.*

if WANT_OVAL
TESTS += OVAL/test_oval.sh 

check_PROGRAMS += test_oval \
	          test_syschar \
		  test_results

test_oval_SOURCES =	OVAL/test_oval.c
test_syschar_SOURCES =	OVAL/test_syschar.c
test_results_SOURCES =	OVAL/test_results.c

EXTRA_DIST += OVAL/test_oval.sh \
	      OVAL/scap-rhel5-oval.xml \
	      OVAL/composed-oval.xml \
	      OVAL/system-characteristics.xml \
	      OVAL/results.xml


if WANT_PROBES
TESTS += OVAL/SEAP/test_seap.sh \
	 OVAL/probes/test_probes.sh

if ENABLE_VALGRIND_TESTS
TESTS += OVAL/probes/vgtest-probes.sh
DISTCLEANFILES += *.vglog
endif

check_PROGRAMS +=	sexp_parser \
			sexp_string \
			sexp_number \
			sexp_list \
			test_probes \
			test_interface \
			test_sysinfo \
			test_spb_api \
			sexp_concurency \
			test_crapi_digest \
			test_crapi_mdigest \
			test_xinetd_parser
#			test_probe-api \
#			test_error \
#			test_probes2

sexp_parser_SOURCES =	OVAL/SEAP/sexp_parser.c
sexp_string_SOURCES =	OVAL/SEAP/sexp_string.c
sexp_number_SOURCES =	OVAL/SEAP/sexp_number.c
sexp_list_SOURCES =	OVAL/SEAP/sexp_list.c
sexp_concurency_SOURCES =     OVAL/SEAP/sexp_concurency.c
sexp_concurency_CFLAGS = @pthread_cflags@
sexp_concurency_LDFLAGS= @pthread_ldflags@
test_spb_api_SOURCES=   OVAL/SEAP/test_spb_api.c
test_spb_api_CFLAGS=    -I$(top_srcdir)/src/OVAL/probes/SEAP/generic
test_probes_SOURCES =	OVAL/probes/test_probes.c
test_probes_CFLAGS =	-I$(top_srcdir)/src/OVAL @libxml_cflags@
test_sysinfo_SOURCES =	OVAL/probes/test_sysinfo.c
test_sysinfo_CFLAGS =	-I$(top_srcdir)/src/OVAL @libxml_cflags@
test_interface_SOURCES = OVAL/probes/test_interface.c
test_interface_CFLAGS =
test_xinetd_parser_SOURCES = OVAL/probes/test_xinetd_parser.c
test_xinetd_parser_CFLAGS= -I$(top_srcdir) -I$(top_srcdir)/src/common -I$(top_srcdir)/src/common/public

test_crapi_digest_SOURCES= OVAL/probes/crapi/test_crapi_digest.c
test_crapi_digest_CFLAGS= -I$(top_srcdir)/src/OVAL/probes/
test_crapi_digest_LDFLAGS= $(top_builddir)/src/OVAL/probes/crapi/libcrapi.la

test_crapi_mdigest_SOURCES= OVAL/probes/crapi/test_crapi_mdigest.c
test_crapi_mdigest_CFLAGS= -I$(top_srcdir)/src/OVAL/probes/
test_crapi_mdigest_LDFLAGS= $(top_builddir)/src/OVAL/probes/crapi/libcrapi.la

# test_probes2_SOURCES =	OVAL/probes/test_probes2.c
# test_error_SOURCES =	OVAL/probes/test_error.c
# test_probe_api_SOURCES =OVAL/probes/test_probe-api.c

EXTRA_DIST += OVAL/SEAP/test_seap.sh 	                    \
	      OVAL/probes/test_probes.sh                    \
	      OVAL/probes/test_probes_file.xml              \
	      OVAL/probes/test_probes_textfilecontent54.xml \
	      OVAL/probes/test_probes_rpminfo.xml.sh        \
	      OVAL/probes/test_probes_runlevel_A.xml.sh     \
	      OVAL/probes/test_probes_runlevel_B.xml.sh     \
	      OVAL/probes/test_probes_password_A.xml.sh     \
	      OVAL/probes/test_probes_shadow_A.xml.sh       \
	      OVAL/probes/test_probes_process_A.xml.sh      \
              OVAL/probes/test_probes_family.xml            \
              OVAL/probes/test_probes_filemd5.xml.sh        \
              OVAL/probes/test_probes_filehash.xml.sh       \
              OVAL/probes/test_probes_interface.xml.sh       \
              OVAL/probes/test_probes_inetlisteningservers.xml.sh \
              OVAL/probes/test_probes_uname.xml.sh \
              OVAL/probes/test_probes_environmentvariable.xml.sh \
	      OVAL/probes/vgtest-probes.sh \
	      OVAL/probes/xinetd_A.conf \
	      OVAL/probes/xinetd_B.conf \
	      OVAL/probes/xinetd_C.conf \
	      OVAL/probes/xinetd_D.conf \
	      OVAL/probes/xinetd_E.conf \
	      OVAL/probes/xinetd_F.conf

if ENABLE_VALGRIND_TESTS
EXTRA_DIST += vgrun.sh
endif

endif # WANT_PROBES 
endif # WANT_OVAL


if WANT_CPE
TESTS += CPE/test_cpeuri.sh  \
	 CPE/test_cpedict.sh \
	 CPE/test_cpelang.sh

check_PROGRAMS +=	test_cpelang \
		 	test_cpedict \
			test_cpeuri

test_cpelang_SOURCES = CPE/test_cpelang.c
test_cpedict_SOURCES = CPE/test_cpedict.c
test_cpeuri_SOURCES = CPE/test_cpeuri.c


EXTRA_DIST +=	CPE/test_cpeuri.sh     \
		CPE/test_cpedict.sh    \
		CPE/test_cpelang.sh    \
		CPE/dict.xml           \
		CPE/lang.xml           \
		CPE/lang-corrupted.xml \
		CPE/lang-damaged.xml   \
		CPE/lang-copy.xml
endif


if WANT_CVE
TESTS += CVE/test_cve.sh
check_PROGRAMS += test_cve
test_cve_SOURCES = CVE/test_cve.c
EXTRA_DIST += CVE/test_cve.sh \
	      CVE/nvdcve-2.0-recent.xml
endif 


if WANT_CCE
TESTS += CCE/test_cce.sh
check_PROGRAMS += test_cce
test_cce_SOURCES = CCE/test_cce.c
EXTRA_DIST += CCE/test_cce.sh            \
	      CCE/cce-sample.xml         \
              CCE/cce-sample-invalid.xml \
              CCE/cce-sample-damaged.xml 
endif


if WANT_CVSS
TESTS += CVSS/test_cvss.sh
check_PROGRAMS += test_cvss
test_cvss_SOURCES = CVSS/test_cvss.c
test_cvss_CFLAGS  = -ffloat-store
EXTRA_DIST +=	CVSS/test_cvsscalc.py \
		CVSS/test_cvss.sh
endif


if WANT_XCCDF
TESTS += XCCDF/test_xccdf.sh
check_PROGRAMS += xccdf_dump test_xccdf
xccdf_dump_SOURCES = XCCDF/xccdf_dump.c
test_xccdf_SOURCES = XCCDF/test_xccdf.c
EXTRA_DIST +=	XCCDF/test_xccdf.sh \
		XCCDF/scap-rhel5-xccdf.xml
endif

