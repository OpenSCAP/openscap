EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = results.xml oscap_debug.log.*
CLEANFILES = results.xml oscap_debug.log.*

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
                  LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas

TESTS = test_probes_family.sh

EXTRA_DIST += test_probes_family.sh test_probes_family.xml
