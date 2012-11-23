EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
                  LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
				  OSCAP_CPE_PATH=$(top_srcdir)/cpe

TESTS = test_sce.sh

EXTRA_DIST +=	test_sce.sh \
		sce_xccdf.xml \
		unconfined_daemons.sh \
		lua_passer.lua \
		python_passer.py
