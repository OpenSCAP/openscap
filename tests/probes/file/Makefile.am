DISTCLEANFILES = *.log results.xml oscap_debug.log.*
CLEANFILES = *.log results.xml verbose oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_file.sh

EXTRA_DIST = test_probes_file.sh \
	test_probes_file.xml \
	test_probes_file_filename.xml

