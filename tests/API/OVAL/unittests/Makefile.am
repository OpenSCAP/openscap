

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*


TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
		LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		OSCAP=$(top_builddir)/utils/.libs/oscap \
		OSCAP_FULL_VALIDATION=1

TESTS = all.sh

check_SCRIPTS = test_applicability_check.sh

EXTRA_DIST += \
	all.sh \
	test_applicability_check.sh \
	applicability_check.xml

