
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

check_DATA = not_executable
not_executable:
	touch not_executable

DISTCLEANFILES = *.log *.out* $(check_DATA)
CLEANFILES = *.log *.out* $(check_DATA)


TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
		LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		OSCAP=$(top_builddir)/utils/.libs/oscap \
		OSCAP_FULL_VALIDATION=1

TESTS = all.sh
check_PROGRAMS = \
	test_oscap_common \
	test_xccdf_shall_pass

test_oscap_common_SOURCES = test_oscap_common.c
test_oscap_common_SOURCES += $(top_srcdir)/src/common/util.c $(top_srcdir)/src/common/list.c $(top_srcdir)/src/common/alloc.c # This needs love (See trac#198)
test_xccdf_shall_pass_SOURCES = test_xccdf_shall_pass.c unit_helper.c

EXTRA_DIST += \
	all.sh \
	test_deriving_xccdf_result_from_oval.sh \
	test_deriving_xccdf_result_from_oval.xccdf.xml \
	test_deriving_xccdf_result_from_oval_pass.oval.xml
	test_deriving_xccdf_result_from_oval_fail.oval.xml
	test_xccdf_complex_check_nand.xccdf.xml \
	test_xccdf_complex_check_single_negate.xccdf.xml \
	test_xccdf_check_content_ref_without_name_attr.oval.xml \
	test_xccdf_check_content_ref_without_name_attr.xccdf.xml \
	test_xccdf_check_content_ref_without_name_attr.sh \
	test_xccdf_check_negate.sh \
	test_xccdf_check_negate.xccdf.xml \
	test_xccdf_check_multi_check.sh \
	test_xccdf_check_multi_check.xccdf.xml \
	test_xccdf_check_multi_check2.sh \
	test_xccdf_check_multi_check2.xccdf.xml \
	test_xccdf_check_multi_check_zero_definitions.sh \
	test_xccdf_check_multi_check_zero_definitions.xccdf.xml \
	test_xccdf_check_processing_complex_priority.sh \
	test_xccdf_check_processing_complex_priority.xccdf.xml \
	test_xccdf_check_processing_invalid_content_refs.sh \
	test_xccdf_check_processing_invalid_content_refs.xccdf.xml \
	test_xccdf_check_processing_selector_empty.sh \
	test_xccdf_check_processing_selector_empty.xccdf.xml \
	test_xccdf_check_processing_selector_bad.sh \
	test_xccdf_check_processing_selector_bad.xccdf.xml \
	test_xccdf_overlaping_IDs.xccdf.xml \
	test_xccdf_refine_rule.sh \
	test_xccdf_refine_rule.xccdf.xml \
	test_xccdf_xml_escaping_value.sh \
	test_xccdf_xml_escaping_value.xccdf.xml \
	oval_without_definitions.oval.xml \
	unit_helper.c \
	unit_helper.h \
	test_xccdf_check_unsupported_check_system.sh \
	test_xccdf_check_unsupported_check_system.xml

