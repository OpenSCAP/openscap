DISTCLEANFILES = *.log *.xml oscap_debug.log.*
CLEANFILES = *.log *.xml oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_environmentvariable.sh

EXTRA_DIST = test_probes_environmentvariable.sh \
	      test_probes_environmentvariable.xml.sh \
	      test_probes_environmentvariable-fail.xml.sh

