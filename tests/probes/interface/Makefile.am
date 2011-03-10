EXTRA_DIST = $(top_srcdir)/tests/test_common.sh 

DISTCLEANFILES = *.log *.xml
CLEANFILES = *.log *.xml

check_PROGRAMS = test_probes_interface
test_probes_interface_SOURCES = test_probes_interface.c

TESTS_ENVIRONMENT= OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so LD_LIBRARY_PATH=$(top_builddir)/src/.libs

TESTS = test_probes_interface.sh

EXTRA_DIST += test_probes_interface.sh test_probes_interface.xml.sh
