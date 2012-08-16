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
		-I$(top_srcdir)/src \
		-I$(top_srcdir)/src/XCCDF_POLICY/public \
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh
		$(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*


TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
		LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		OSCAP=$(top_builddir)/utils/.libs/oscap

TESTS = all.sh
check_PROGRAMS = test_xccdf_shall_pass
check_SCRIPTS = \
	test_xccdf_check_content_ref_without_name_attr.sh \
	test_xccdf_xml_escaping_value.sh

test_xccdf_shall_pass_SOURCES = test_xccdf_shall_pass.c unit_helper.c

EXTRA_DIST += \
	all.sh \
	not_executable \
	test_xccdf_complex_check_nand.xccdf.xml \
	test_xccdf_complex_check_single_negate.xccdf.xml \
	test_xccdf_check_content_ref_without_name_attr.oval.xml \
	test_xccdf_check_content_ref_without_name_attr.xccdf.xml \
	test_xccdf_check_content_ref_without_name_attr.sh \
	test_xccdf_overlaping_IDs.xccdf.xml \
	test_xccdf_xml_escaping_value.sh \
	test_xccdf_xml_escaping_value.xccdf.xml \
	unit_helper.c \
	unit_helper.h
