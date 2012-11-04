EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log results.xml
CLEANFILES = \
	*.log \
	*results.xml

TESTS_ENVIRONMENT = \
	LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
	LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
	OSCAP=$(top_builddir)/utils/.libs/oscap \
	OSCAP_FULL_VALIDATION=1 \
	OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
	OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
	XPATH=$(top_srcdir)/tests/xpath.pl

TESTS = all.sh

EXTRA_DIST += \
	all.sh \
	test_probes_textfilecontent54.sh \
	test_probes_textfilecontent54.xml \
	test_validation_of_various_oval_versions.sh \
	tfc54-def-5.4-invalid.xml \
	tfc54-def-5.4-valid.xml \
	tfc54-def-5.5-valid.xml \
	tfc54-def-5.6-valid.xml \
	tfc54-def-5.7-valid.xml \
	tfc54-def-5.8-valid.xml \
	tfc54-def-5.9-valid.xml \
	tfc54-def-5.10-valid.xml \
	tfc54-def-5.10.1-valid.xml
