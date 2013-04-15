EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.xml
CLEANFILES = *.log *.xml

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_process.sh

EXTRA_DIST += test_probes_process.sh test_probes_process.xml.sh
