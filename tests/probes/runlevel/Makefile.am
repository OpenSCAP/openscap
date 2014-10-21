DISTCLEANFILES = *.log *.xml oscap_debug.log.*
CLEANFILES = *.log *.xml oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_runlevel.sh

EXTRA_DIST = test_probes_runlevel.sh \
              runlevel_helper.sh \
              test_probes_runlevel_A.xml.sh \
              test_probes_runlevel_B.xml.sh \
              test_probes_runlevel_C.xml.sh
