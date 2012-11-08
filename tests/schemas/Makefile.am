EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

# We use absolute paths for LD_* because relative paths would break tests
# whenever cd/pushd is used in them.
TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(shell cd $(top_builddir); pwd)/src/OVAL/probes \
                  LD_PRELOAD=$(shell cd $(top_builddir); pwd)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(shell cd $(top_builddir); pwd)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(shell cd $(top_srcdir); pwd)/schemas \
                  OSCAP_FULL_VALIDATION=1

TESTS = all.sh

EXTRA_DIST += all.sh
