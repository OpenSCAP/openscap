DISTCLEANFILES = *.log *.xml oscap_debug.log.*
CLEANFILES = *.log *.xml oscap_debug.log.*

check_PROGRAMS = test_probes_interface
test_probes_interface_SOURCES = test_probes_interface.c

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_interface.sh

EXTRA_DIST = test_probes_interface.sh test_probes_interface.xml.sh
