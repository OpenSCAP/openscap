DISTCLEANFILES = *.log *.xml oscap_debug.log.*
CLEANFILES = *.log *.xml oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_rpmverifypackage.sh

EXTRA_DIST = test_probes_rpmverifypackage.sh test_probes_rpmverifypackage.xml.sh
