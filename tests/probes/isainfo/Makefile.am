DISTCLEANFILES = oscap_debug.log.*
CLEANFILES = oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_isainfo.sh

EXTRA_DIST =	test_probes_isainfo.sh \
		test_probes_isainfo.xml

