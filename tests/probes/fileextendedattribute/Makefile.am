DISTCLEANFILES = *.log results.xml
CLEANFILES = *.log results.xml

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_fileextendedattribute.sh

EXTRA_DIST = test_probes_fileextendedattribute.sh test_probes_fileextendedattribute.xml
