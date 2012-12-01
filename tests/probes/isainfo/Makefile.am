EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = oscap_debug.log.*
CLEANFILES = oscap_debug.log.*

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
                  LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		builddir=$(top_builddir) \
		  OSCAP_FULL_VALIDATION=1

TESTS = test_probes_isainfo.sh

EXTRA_DIST +=	test_probes_isainfo.sh \
		test_probes_isainfo.xml

