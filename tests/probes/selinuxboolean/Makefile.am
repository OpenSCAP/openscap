DISTCLEANFILES = *.log *.xml oscap_debug.log.*
CLEANFILES = *.log *.xml oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_selinuxboolean.sh

EXTRA_DIST = test_probes_selinuxboolean.sh \
	      test_probes_selinuxboolean.xml.sh \
	      test_probes_selinuxboolean-fail.xml.sh

