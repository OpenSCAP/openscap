EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

# We use absolute paths for LD_* because relative paths would break tests
# whenever cd/pushd is used in them.
TESTS_ENVIRONMENT=LD_PRELOAD=$(shell cd $(top_builddir); pwd)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(shell cd $(top_builddir); pwd)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(shell cd $(top_srcdir); pwd)/schemas

TESTS = test_ds.sh

EXTRA_DIST += test_ds.sh \
		empty/empty-xccdf.xml \
		simple/scap-fedora14-xccdf.xml \
		simple/scap-fedora14-oval.xml \
		multiple_oval/multiple-oval-xccdf.xml \
		multiple_oval/first-oval.xml \
		multiple_oval/second-oval.xml
