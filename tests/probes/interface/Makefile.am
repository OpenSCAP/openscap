EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.xml
CLEANFILES = *.log *.xml

check_PROGRAMS = test_probes_interface
test_probes_interface_SOURCES = test_probes_interface.c

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_interface.sh

EXTRA_DIST += test_probes_interface.sh test_probes_interface.xml.sh
