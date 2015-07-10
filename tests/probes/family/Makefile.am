DISTCLEANFILES = *.log results.xml oscap_debug.log.*
CLEANFILES = *.log results.xml oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_family.sh

EXTRA_DIST = test_probes_family.sh test_probes_family.xml
