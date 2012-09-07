

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

check_SCRIPTS = test_applicability_check.sh \
		test_empty_filename.sh \
		test_deprecated_def.sh \
		test_count_function.sh \
		test_item_not_exist.sh

EXTRA_DIST += \
	all.sh \
	test_applicability_check.sh \
	applicability_check.xml \
	test_empty_filename.sh \
	empty_filename.xml \
	test_deprecated_def.sh \
	deprecated_def.xml \
	test_count_function.sh \
	oval-def_count_function.xml \
	test_item_not_exist.sh \
	item_not_exist.xml

