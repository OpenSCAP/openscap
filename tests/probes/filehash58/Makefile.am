DISTCLEANFILES = *.log *.xml oscap_debug.log.*
CLEANFILES = *.log *.xml oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_filehash58.sh

EXTRA_DIST = test_probes_filehash58.sh test_probes_filehash58.xml.sh check_filehash_simple.xml
