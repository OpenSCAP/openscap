EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = \
	*.log \
	*.out.??????
CLEANFILES = $(DISTCLEANFILES)

TESTS_ENVIRONMENT = \
	LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
	LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
	OSCAP=$(top_builddir)/utils/.libs/oscap \
	OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
	OSCAP_FULL_VALIDATION=1 \
	OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes

if WANT_PROBES_UNIX
if probe_file_enabled
TESTS = all.sh
endif
endif

check_SCRIPTS = test_object_entity_mask.sh test_object_entity_nomask.sh

EXTRA_DIST += \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	test_object_entity_mask.sh \
	test_object_entity_mask.xml \
	test_object_entity_nomask.sh \
	test_object_entity_nomask.xml
