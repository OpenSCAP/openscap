EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT=LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas

TESTS = test_ds.sh

EXTRA_DIST += test_ds.sh \
		empty/empty-xccdf.xml \
		simple/scap-fedora14-xccdf.xml \
		simple/scap-fedora14-oval.xml

