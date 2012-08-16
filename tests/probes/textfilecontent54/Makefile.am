EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log results.xml
CLEANFILES = *.log results.xml

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
                  LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		  OSCAP_FULL_VALIDATION=1

TESTS = test_probes_textfilecontent54.sh

EXTRA_DIST += \
	test_probes_textfilecontent54.sh \
	test_probes_textfilecontent54.xml \
	tfc54-def-5.4-invalid.xml \
	tfc54-def-5.4-valid.xml \
	tfc54-def-5.5-valid.xml \
	tfc54-def-5.6-valid.xml \
	tfc54-def-5.7-valid.xml \
	tfc54-def-5.8-valid.xml \
	tfc54-def-5.9-valid.xml \
	tfc54-def-5.10-valid.xml \
	tfc54-def-5.10.1-valid.xml
