EXTRA_DIST = $(top_srcdir)/tests/test_common.sh  

DISTCLEANFILES = *.log *.xml
CLEANFILES = *.log *.xml

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
                  LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas

TESTS = test_probes_environmentvariable.sh

EXTRA_DIST += test_probes_environmentvariable.sh \
	      test_probes_environmentvariable.xml.sh
